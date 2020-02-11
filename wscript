#!/usr/bin/env python


TOP = '.'
APPNAME = 'Junk'

from waflib.extras import wcb
wcb.package_descriptions.update(
    dict(
        WCT = dict(
            incs=["WireCellUtil/Units.h","WireCellIface/IConfigurable.h"],
            libs=["WireCellUtil","WireCellIface"],
            extuses=("JSONCPP",), # should not have to specify!
            mandatory=True),
        ZMQ = dict(
            incs=["zmq.h"],
            libs=['zmq'], 
            pcname='libzmq',
            mandatory=True),
        CZMQ = dict(
            incs=["czmq.h"],
            libs=['czmq'],
            pcname='libczmq',
            mandatory=True),
        ZYRE = dict(
            incs=["zyre.h"],
            libs=['zyre'],
            pcname='libzyre',
            mandatory=True),
        ZIO = dict(
            incs=["zio/node.hpp"],
            libs=['zio'],
            extuses=("ZYRE","CZMQ","ZMQ"),
            mandatory=True,
            )))
del wcb.package_descriptions["Jsonnet"]

def options(opt):
    opt.load("wcb")
    opt.load('protobuf')

def configure(cfg):
    cfg.load("wcb")
    cfg.load('protobuf')

def build(bld):
    bld.load('wcb')
    bld.smplpkg('WireCellZpb',
                use='WCT ZIO ZMQ PROTOBUF EIGEN JSONCPP',
                test_use='TBB')

