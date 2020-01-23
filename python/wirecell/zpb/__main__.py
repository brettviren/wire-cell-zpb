#!/usr/bin/env python3
'''
Main CLI for wirecell.zpb
'''
import json
from . import jsonnet
import click
from rule import Rule           # bv's hacked version
from . import lispish
import logging
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s.%(msecs)03d %(levelname)s\t%(message)s',
                    datefmt='%Y-%m-%d %H:%M:%S')
log = logging.getLogger("zpb")

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
        rule_attr = dict(msg_attr)
        rule_attr.update(robj.get('attr',{}))
        log.debug(f'rule attributes: {rule_attr}')
        parser = lispish.parser(rule_attr)
        sexp = robj['rule']
        try:
            parsed = parser.parseString(sexp)
        except KeyError as e:
            log.error(f'missing parameter in rule {ind}: {e}\n\trule: {sexp}\n\tattr: {rule_attr}')
            continue
        except lispish.ParseException:
            log.critical(f'parse error with rule:\n{sexp}')
            raise
        parsed = parsed[0]
        log.debug(f'parsed expression: {parsed}')
        expr = Rule(parsed, return_bool = True)
        log.debug(f'rule expression: {expr}')
        tf = expr.match();
        filepat = robj['filepat']
        try:
            path = filepat.format(**rule_attr)
        except KeyError as e:
            log.error(f'missing parameter: {e}, filepat: "{filepat}"')
            continue

        grouppat = robj['grouppat']
        try:
            group = grouppat.format(**rule_attr)
        except KeyError as e:
            log.error(f'missing parameter: {e}, grouppat: "{grouppat}"')
            continue

        rw = robj['rw']
        tf = "TRUE" if tf else "FALSE"
        log.info(f'#{ind} {tf:5s} {rw} {path}:/{group}')
        

@cli.command("file-server")
@click.option("-r","--ruleset",type=click.Path(),
              help="A file in JSON or Jsonnet format providing the ruleset")
@click.argument("filename", nargs=-1)
def file_server(filename):
    '''
    Serve files over ZPB/ZIO.
    '''
    
def main():
    cli(obj=dict())

if '__main__' == __name__:
    main()
    
