#!/usr/bin/env python3
'''
ZBP factories are used to service client flows.
'''

from .util import message_to_dict
from . import rules

import logging
log = logging.getLogger("zpb")

def wash_one(t):
    return t if isinstance(t,tuple) else (t,())
def wash(tt):
    return tuple(map(wash_one, tt))


class Ruleset:
    '''Use a ruleset to mate flow client to a file resource.

    The ruleset factory will apply each rule in the set and used the
    first one that evaluates True.  This will govern the parameters
    for the file resource.

    The file resource is delegated to a pair of actor functions for
    reading and another for writing.  A single reader/writer actor is
    launched to handle each new file and a reader or writer handler is
    used to handle each new client.

    '''

    def __init__(self, ctx, ruleset, 
                 wactors=(), ractors=()):
        '''Create a Factory with a ruleset.

        Parameters
        ----------

        ctx : a zmq.Context

        ruleset: array of dictionary

            A ruleset is a sequence of rule specifications, each of
            which is a dictionary with these attributes:

        wactors : 2-tuple specifing file writing actors 

            See below.

        ractors : 2-tuple specifying file reading actors

            See below

        The "*actors" args are 2-tuples specifying actor functions an
        optionally user data arguments for writing and reading.

        The head of the 2-tuple is for the file actor which directly
        manages some file resource.  The tail is for the client actor
        which will connect to the file actor socket (and presumably
        some other socket).  If either element of this 2-tuple is
        itself a tuple then the head is treated as the actor function
        and any remainder is passed to the function as user data
        arguments.

        A file actor is called as:

            file_handler(ctx, pipe, 
                         filename,
                         *user_data):

        The file actor function must adhere to a simple protocol on
        its actor pipe.  After issuing the usual actor ready signal
        and before entering its loop, the file actor shall return on
        its actor pipe as a string the address of a bound socket to
        which any subsequent client actors may connect.

        A client actor is called as:

            client_handler(ctx, pipe, 
                           bot, rule_object, file_addr, 
                           *user_data)

        The client actor has no protocol on its actor pipe other than
        the usual ready signal.

        Rule Attributes
        ---------------

        rule : string

            An S-expression in terms of attributes evaluating to a
            Boolean.  First rule returning True is used.  Here and
            below "attributes" refer to the combination of BOT message
            header values and any additional specified in the `attr`
            Rule Attribute (described below).

        rw : string 

            Takes value "read" or "write" and determines if the flow
            is to be read from or written to file.

        filepat : f-string

            An f-string formatted against the attributes which
            resolves to the name of the file to be read or written.

        grouppat : f-string

            An f-string formatted against attributes to determine and
            to add a "hdfgroup" parameter to the BOT message prior to
            it being sent into a read or write handler.  The value
            will be used to define an HDF base group path into which
            messages from one stream will be mapped.  This path must
            be unique to the flow client.

        attr : dictionary
        
            An extra dictionary of attributes updated on the
            dictionary of message attributes prior to applying to the
            rule or either patterns.

        '''
        self.ctx = ctx
        self.ruleset = ruleset
        self.wactors = wash(wactors)
        self.ractors = wash(ractors)
        self.writers = dict()
        self.handlers = list()
        return

    def __call__(self, bot):
        '''
        Given a bot, return a live actor or None if bot is rejected.
        '''
        attr = message_to_dict(bot)
        
        for maybe in self.ruleset:
            log.debug(f'check rule: "{maybe}"')
            parsed = rules.parse(maybe, **attr)
            log.debug ("parsed:",parsed)
            tf = rules.evaluate(parsed)
            if not tf:
                log.debug ("rule does not apply")
                continue
            rattr = dict(maybe["attr"], **attr)
            filename = maybe["filepat"].format(**rattr)
            rw = maybe["rw"][0]
            log.debug(f'{filename} ({rw})')

            if rw == 'r':
                self.launch_read(filename, bot, maybe)
                return
            if rw == 'w':
                self.launch_write(filename, bot, maybe)
                return
            continue

    def launch_read(self, filename, bot, rule):
        pass

    def launch_write(self, filename, bot, rule):

        # launch file actor if needed
        try:
            wfile = self.writers[filename]
        except KeyError:
            wactor, wargs = self.wactors[0]
            wfile = ZActor(self.ctx, wactor, filename, *wargs)
            waddr = wfile.pipe.recv_string()
            if not waddr:
                err = f"failed to bind any {self.addrpat} for {filename}"
                log.error(err)
                raise RuntimeError(err)
            wfile.addr = waddr # copascetic?
            log.debug(f"made writer actor for {filename}")
            self.writers[filename] = wfile

        # the client handler
        wactor, wargs = self.wactors[1]

        actor = ZActor(self.ctx, wactor,
                       bot, rule, 
                       wfile.addr, *wargs)
        self.handlers.append(actor)
        return True

    def stop(self):
        log.debug('stop %d handlers' % len(self.handlers))
        for handler in self.handlers:
            handler.pipe.signal()
        for filename, wactor in self.writers.items():
            log.debug(f'stop writer for {filename}')
            wactor.pipe.signal()
