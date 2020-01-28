#!/usr/bin/env python3
'''Support for writing HDF5 files.

It provides:

- a Writer class with a mapping policy from message to HDF group

- a file handler actor function

- a data flow client handler actor function

Note, this file is not dependent on any message nor message-dependent
HDF schema.  That is delegated to other modules.

'''

import json

from zio import Port, Message
from zio.flow import objectify, Flow
from pyre.zactor import ZActor
from zmq import CLIENT, PUSH, PULL, Poller, POLLIN
import h5py
from ..util import message_to_dict

from google.protobuf.any_pb2 import Any 
import logging
log = logging.getLogger("zpb")


class Writer:
    '''Write ZIO messages to HDF5.

    The writer is given a base group into which data from subsequent
    messages are written.  

    The message information is placed in a subgroup:

        <seqno>/<index>/[<datasets>]

    Path elements:
    ---------------
    seqno : int
        The seqno attribute from the message coord header.
    index : int
        Zero-based index into the message payload array

    The `<seqno>` group holds as HDF `attrs` the flow object (prefix
    header label) as well as `origin` and `granule` values from the
    coord header.  

    Zero or more datasets may be defined to hold array-like
    information and they may have HDF `attrs` set.
    '''

    def __init__(self, group, pbmod, tohdfmod):
        self.group = group
        self.pbmod = pbmod
        self.tohdf = tohdfmod

    def save(self, msg):
        fobj = objectify(msg)
        gn = str(msg.seqno)
        seq = self.group.get(gn, None)
        if seq is None:         # as it should be
            seq = self.group.create_group(gn)
        else:
            log.error(f'HDF5 writer cowardly refusing to use existing group {gn}')
            return
        seq.attrs["origin"] = msg.origin
        seq.attrs["granule"] = msg.granule
        for k,v in fobj.items():
            if k in ["flow", "direction"]:
                continue
            seq.attrs[k] = v
        
        for index, data in enumerate(msg.payload):
            a = Any()
            size = a.ParseFromString(data)
            assert(size>0)
            tn = a.TypeName()
            log.debug(f"payload {index} of size {size} and type {tn}")
            type_name = tn.rsplit('.',1)[-1]
            PBType = getattr(self.pbmod, type_name)
            pbobj = PBType()
            ok = a.Unpack(pbobj)
            if not ok:
                err = f'fail to unpack protobuf object of type {tn}'
                log.error(err)
                raise RuntimeError(err)
            tohdf = getattr(self.tohdf, type_name)
            slot = seq.create_group(str(index))
            tohdf(pbobj, slot)

def file_handler(ctx, pipe, filename, addrpat, wargs):
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

def client_handler(ctx, pipe, bot, rule_object, writer_addr, broker_addr):
    '''Connect to and marshall messages between broker and writer sockets.

    Parameters
    ----------

    bot : zio.Message

        The BOT message

    rule_object: dicionary 

        A ruleset rule object.

    writer_addr :: string

        The address of the writer's PULL socket to connect.

    broker_addr : string

        The address of the broker's SERVER socket to connect.

    '''
    # An HDF path to be added to every message we send to writer.
    mattr = message_to_dict(bot)
    rattr = dict(rule_object["attr"], **mattr)
    base_path =  rule_object["grouppat"].format(**rattr)
    log.debug(f'client_handler(msg, "{base_path}", "{broker_addr}", "{writer_addr}")')
    log.debug(bot)
    pipe.signal()

    push = ctx.socket(PUSH)
    push.connect(writer_addr)

    sock = ctx.socket(CLIENT)
    port = Port("write-handler", sock)
    port.connect(broker_addr)
    port.online(None)
    flow = Flow(port)
    log.debug (f'client_handler({base_path}) send BOT to {broker_addr}')
    flow.send_bot(bot)          # this introduces us to the server
    bot = flow.recv_bot()
    log.debug (f'client_handler({base_path}) got response:\n{bot}')
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

