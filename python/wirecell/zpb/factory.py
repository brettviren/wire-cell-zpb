#!/usr/bin/env python
'''
A flow broker factory for providing an HDF file service.

It applies a rule set to each BOT and constructs a matching actor.

For client extraction flows, the actors simply forward messages to
their PUSH and the connected PULL in a writing actor writes to file.

For client inject flows, .... something else happens.  TBD.
'''
import json
from rule import Rule           # bv's hacked version
import lispish
from zio import Port, Message
from zio.flow import objectify, Flow
from pyre.zactor import ZActor
from zmq import CLIENT, PUSH, PULL, Poller, POLLIN
import h5py
from writer import Writer

import logging
log = logging.getLogger(__name__)

def message_to_dict(msg):
    '''
    Return a simple dictionary of message header info.
    '''
    d = objectify(msg)
    d['origin'] = msg.origin
    d['granule'] = msg.granule
    d['seqno'] = msg.seqno
    return d

def writer(ctx, pipe, filename, addrpat, wargs):
    '''An actor that marshals messages from socket to file.

    Parameters
    -----------

    filename : string

        Name of an HDF file in which to write

    addrpat : string

        An f-string formatted with a "port" parameter that should
        resolve to a legal ZeroMQ socket address.  When a successful
        bind() can be done on the result, the resolved address is
        returned through the pipe.  If no successful address can be
        bound, an empty string is returned as an error indicator.

    wargs : tuple

        Args passed to Writer.

    '''
    log.debug(f'actor: writer("{filename}", "{addrpat}")')
    fp = h5py.File(filename,'w')
    log.debug(f'opened {filename}')
    pipe.signal()
    log.debug('make writer PULL socket')
    pull = ctx.socket(PULL)
    minport,maxport = 49999,65000
    for port in range(minport,maxport):
        writer_addr = addrpat.format(port=port)
        pull.bind(writer_addr)
        log.debug(f'writer bind to {writer_addr}')
        pipe.send_string(writer_addr)
        break

    flow_writer = dict()

    poller = Poller()
    poller.register(pipe, POLLIN)
    poller.register(pull, POLLIN)

    while True:
        for which,_ in poller.poll():
            if not which:
                return
            if which == pipe: # signal exit
                log.debug(f'writer for {filename} exiting')
                return

            # o.w. we have flow

            data = pull.recv()
            if not data:
                continue
            msg = Message(encoded=data)
            fobj = objectify(msg)
            path = fobj.pop("hdfgroup") # must be supplied
            msg.label = json.dumps(fobj)
            log.debug(f'{filename}:/{path} writing:\n{msg}')

            fw = flow_writer.get(path, None)
            if fw is None:
                sg = fp.get(path, None) or fp.create_group(path)
                fw = flow_writer[path] = Writer(sg, *wargs)

            fw.save(msg)
        

    return

def write_handler(ctx, pipe, bot, base_path, broker_addr, writer_addr):
    '''Connect to and marshall messages between broker and writer.

    Parameters
    ----------

    bot : zio.Message

        The BOT message

    base_path : string

        An HDF path to be added to every message we send to writer.

    broker_addr : string

        The address of the broker's SERVER socket to connect.

    writer_addr :: string

        The address of the writer's PULL socket to connect.

    '''
    log.debug(f'actor: write_handler(msg, "{base_path}", "{broker_addr}", "{writer_addr}")')
    log.debug(bot)
    pipe.signal()

    push = ctx.socket(PUSH)
    push.connect(writer_addr)

    sock = ctx.socket(CLIENT)
    port = Port("write-handler", sock)
    port.connect(broker_addr)
    port.online(None)
    flow = Flow(port)
    log.debug (f'write_handler({base_path}) send BOT to {broker_addr}')
    flow.send_bot(bot)          # this introduces us to the server
    bot = flow.recv_bot()
    log.debug (f'write_handler({base_path}) got response:\n{bot}')
    flow.flush_pay()

    def push_message(m):
        log.debug (f'write_handler({base_path}) push {m}')
        attr = message_to_dict(m)
        attr['hdfgroup'] = base_path
        m.label = json.dumps(attr)
        push.send(m.encode())

    push_message(bot)

    poller = Poller()
    poller.register(pipe, POLLIN)
    poller.register(sock, POLLIN)
    while True:

        for which,_ in poller.poll():
            if not which:
                return

            if which == pipe: # signal exit
                log.debug ('write_handler pipe hit')
                return          

            # o.w. we have flow

            msg = flow.get()
            if not msg:
                flow.send_eot()
                # fixme: send an EOT also to push socket?.
                break

            if msg.form != 'FLOW':
                continue

            fobj = objectify(msg)
            ftype = fobj.get("flow", None)
            if not ftype:
                continue

            # fixme: maybe add some to fobj here and repack

            push_message(msg)

            if ftype == 'EOT':
                flow.send_eot()

            # fixme: we might get fresh BOT, check for it.

            # fixme: how do we exit?
            continue
    log.debug ('write_handler exiting')

class Factory:

    def __init__(self, ctx, ruleset, broker_addr,
                 addrpat = "tcp://127.0.0.1:{port}", wargs=()):
        '''Create a Factory with a ruleset.

        Parameters
        ----------

        addrpat : string

            F-string pattern formated with a "port" to produce a
            ZeroMQ address to bind.  Used by writer or reader actors.

        broker_addr : string

            A ZeroMQ address string of the broker to which handlers
            should connect.

        ruleset: array of dictionary

            A ruleset is a sequence of rule specifications, each of
            which is a dictionary with these attributes:

        wargs : tuple

            Arguments passed to Writer

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
        self.broker_addr = broker_addr
        self.writers = dict()
        self.handlers = list()
        self.addrpat = addrpat
        self.wargs = wargs
        return

    def __call__(self, bot):
        '''
        Given a bot, return a live actor or None if bot is rejected.
        '''
        attr = message_to_dict(bot)
        
        for maybe in self.ruleset:
            rattr = dict(attr, **maybe["attr"])
            rule = maybe["rule"]
            parser = lispish.sexp(rattr)
            parsed = parser.parseString(rule)
            log.debug ("parsed:",parsed)
            expr = Rule(parsed, return_bool = True)
            if not expr.match():
                continue;
            filename = maybe["filepat"].format(**rattr)



            if maybe["rw"].startswith('r'):
                continue        # fixme: we only support writing so far
            # else: write

            # just in time construction of writer for a given file
            try:
                wactor = self.writers[filename]
            except KeyError:
                wactor = ZActor(self.ctx, writer, filename, self.addrpat, self.wargs)
                waddr = wactor.pipe.recv_string()
                if not waddr:
                    err = f"failed to bind any {self.addrpat} for {filename}"
                    raise RuntimeError(err)
                wactor.addr = waddr # copascetic?
                log.debug(f"zpbhdf.Factory: make writer for {filename}")
                self.writers[filename] = wactor

            base_path = maybe["grouppat"].format(**rattr)
            actor = ZActor(self.ctx, write_handler, bot, base_path,
                           self.broker_addr,
                           self.writers[filename].addr)
            self.handlers.append(actor)
            return True
        return
    def stop(self):
        log.debug('stop %d handlers' % len(self.handlers))
        for handler in self.handlers:
            handler.pipe.signal()
        for filename, wactor in self.writers.items():
            log.debug(f'stop writer for {filename}')
            wactor.pipe.signal()
