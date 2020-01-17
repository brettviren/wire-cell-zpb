#!/usr/bin/env python3

from setuptools import setup
setup(
    name="wirecell_zpb",
    packages=['wirecell.zpb'],
    author='Brett Viren',
    author_email='bv@bnl.gov',
    version = '0.0.1',
    zip_safe = False,
    install_requires = [
        "rule==0.1.2bv",
        "zio==0.0.0",
        "h5py",
        "protobuf",
    ],
)
