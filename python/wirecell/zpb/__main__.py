#!/usr/bin/env python3
'''
Main CLI for wirecell.zpb
'''

import click


@click.group("zpb")
@click.pass_context
def cli(ctx):
    '''
    Wire Cell ZIO/ZeroMQ/Protobuf CLI.
    '''

@cli.command("file-server")
@click.argument("filename", nargs=-1)
def file_server(filename):
    '''
    Serve files over ZPB/ZIO.
    '''
    
