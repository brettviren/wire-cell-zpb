#!/usr/bin/env python3
'''
Main CLI for wirecell.zpb
'''
import json
from . import jsonnet
import click
from rule import Rule           # bv's hacked version
from . import lispish

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
@click.argument("attrs", nargs=-1)
def test_ruleset(ruleset, attrs):
    '''
    Test a rule set by giving attributes as key=value on command line.
    '''
    msg_attr = dict()
    for attr in attrs:
        try:
            k,v = attr.split('=', 1)
        except ValueError:
            print(f'failed to parse {attr}')
            continue
        msg_attr[k] = typeit(v)

    rs = jsonnet.load(ruleset)
    # fixme: move this to a module
    for ind,robj in enumerate(rs):
        rule_attr = dict(msg_attr)
        rule_attr.update(robj.get('attr',{}))
        print (rule_attr)
        parser = lispish.sexp(rule_attr)
        sexp = robj['rule']
        try:
            parsed = parser.parseString(sexp)
        except lispish.pp.ParseException:
            print (sexp)
            raise

        expr = Rule(parsed, return_bool = True)
        tf = expr.match();
        path = robj['filepat'].format(**rule_attr)
        group = robj['grouppat'].format(**rule_attr)
        click.echo(f'{ind} {tf} {robj.rw} {path} {group}')
        

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
    
