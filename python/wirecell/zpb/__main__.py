#!/usr/bin/env python3
'''
Main CLI for wirecell.zpb
'''
import logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s.%(msecs)03d %(levelname)s\t%(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S')
log = logging.getLogger("zpb")

import json
from . import jsonnet
import click
from . import rules

@click.group("zpb")
@click.pass_context
def cli(ctx):
    '''
    Wire Cell ZIO/ZeroMQ/Protobuf CLI.
    '''


def typeit(v):
    try:
        return int(v)
    except ValueError:
        pass
    try:
        return float(v)
    except ValueError:
        pass
    return v                    # fixme: should recurs...

@cli.command("test-ruleset")
@click.option("-r","--ruleset",type=click.Path(),
              help="A file in JSON or Jsonnet format providing the ruleset")
@click.option("-v","--verbosity", default="info",
              help="Set logging level (debug, info, warning, error, critical)")
@click.argument("attrs", nargs=-1)
def test_ruleset(ruleset, verbosity, attrs):
    '''
    Test a rule set by giving attributes as key=value on command line.
    '''
    log.level = getattr(logging, verbosity.upper(), "INFO")

    msg_attr = dict()
    for attr in attrs:
        try:
            k,v = attr.split('=', 1)
        except ValueError:
            log.error(f'failed to parse {attr}')
            continue
        msg_attr[k] = typeit(v)

    rs = jsonnet.load(ruleset)
    # fixme: move this to a module
    for ind,robj in enumerate(rs):
        attr = dict(robj.get('attr',{}), **msg_attr)
        log.debug(f'attr: {attr}')

        def dump(errtype, err):
            log.error(f'{errtype} "{err}"')
            log.error('\n%s' % (robj['rule'],))
            log.error('\n%s' % (robj['attr'],))
            log.error('\n%s' % (attr,))
            

        # do parsing
        try:
            parsed = rules.parse(robj, **attr)
        except KeyError as ke:
            dump("key error", ke)
            continue

        # do evaluating
        # can call rules.evaluate() but we want to print extra stuff here
        log.debug(f'parsed expression: {parsed}')
        expr = rules.Rule(parsed) #, return_bool = True)
        log.debug(f'rule expression: {expr}')
        tf = expr.match();
        log.debug(f'rule evaluation: {tf}')

        # do string interpolation no the "pat" patterns
        filepat = robj['filepat']
        try:
            path = filepat.format(**attr)
        except KeyError as ke:
            dump(f'key error "{filepat}"', ke)
            continue

        grouppat = robj['grouppat']
        try:
            group = grouppat.format(**attr)
        except KeyError as ke:
            dump(f'key error "{grouppat}"', ke)
            continue

        rw = robj['rw']
        tf = "TRUE" if tf else "FALSE"
        log.info(f'#{ind} {tf:5s} {rw} {path}:/{group}')
        

@cli.command("file-server")
@click.option("-r","--ruleset", type=click.Path(), required=True,
              help="A file in JSON or Jsonnet format providing the ruleset")
@click.option("-b","--bind", default="tcp://127.0.0.1:22351",
              help="An address to which the server shall bind")
@click.option("-f","--format", default="hdf", type=click.Choice(["hdf"]),
              help="File format")
@click.option("-n","--name", default="zpbfiles",
              help="The Zyre node name for the server")
@click.option("-p","--port", default="flow",
              help="The port name for the server socket")
@click.option("-v","--verbosity", default="info",
              help="Set logging level (debug, info, warning, error, critical)")
def file_server(ruleset, bind, format, name, port, verbosity):
    '''
    Serve files over ZPB/ZIO.
    '''
    import zmq
    from zio import Port, Message, Node
    from zio.flow.broker import Broker
    from .factory import Ruleset as Factory
    from . import pb as pbmod
    from .jsonnet import load as jsonnet_load

    # for now we only support HDF
    from .hdf import writer, reader, frompb, topb
    assert(format == "hdf")

    log.level = getattr(logging, verbosity.upper(), "INFO")

    ruleset = jsonnet_load(ruleset)

    # fixme: is it really needed to create a common ctx?
    ctx = zmq.Context()
    factory = Factory(ctx, ruleset, 
                      wactors=((writer.file_handler,
                                ("inproc://hdfwriter{port}", (pbmod, frompb))),
                               (writer.client_handler,
                                (bind,))),
                      ractor=(reader.handler,(bind, pbmod, topb)))

    node = Node(name)
    sport = node.port(port, zmq.SERVER)
    sport.bind(bind)
    node.online()    
    log.info(f'broker {name}:{port} online at {bind}')

    broker = Broker(sport, factory)

    log.info(f'broker {name} entering loop')
    while True:
        try:
            broker.poll(10000)
        except TimeoutError:
            node.peer.drain()
            log.debug(f'broker {name} is lonely')
            log.debug(node.peer.peers)
        except Exception as e:
            log.critical(e)
            raise
            break

    broker.stop()

    
def main():
    cli(obj=dict())

if '__main__' == __name__:
    main()
    
