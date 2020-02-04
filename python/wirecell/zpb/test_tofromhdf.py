#!/usr/bin/env python3
'''
Test conversion to/from PB and HDF
'''

import os
import h5py
import time
import numpy
import shutil
import random
import tempfile
from wirecell.zpb.hdf import topb, frompb
from wirecell.zpb import pb

def test_depo():
    'Test depo conversion'

    depo1 = pb.Depo(ident=99,
                    pos=pb.Point(x=1,y=2,z=3),
                    time=100.0, charge=1000.0,
                    trackid=12345, pdg=11,
                    extent_long=9.6, extent_tran=6.9) 

    tmpdir = tempfile.mkdtemp()
    hdf = h5py.File(os.path.join(tmpdir, "test_depo.hdf"), 'w')
    frompb.Depo(depo1, hdf)
    depo2 = topb.Depo(hdf, pb)

    for field in "ident time charge trackid pdg pos".split():
        assert(getattr(depo1, field) == getattr(depo2, field))

    for field in "extent_long extent_tran".split():
        e1 = float(getattr(depo1, field))
        e2 = float(getattr(depo2, field))
        assert(abs(e1-e2) < 1e-6)
    shutil.rmtree(tmpdir)

def test_sparse_frame():
    'Test sparse frame conversion'

    t1 = time.time()
    frame1 = pb.Frame(ident=2, time=time.time(), tick=500.0)
    chanset = list(range(1000))
    negs = frame1.tagged_traces["negs"].elements
    negs_tot = frame1.trace_summaries["negs"].elements
    for ind in range(500):
        chan = random.choice(chanset)
        chanset.remove(chan)
        tbin = int(random.uniform(0,1000))
        nticks = int(random.uniform(tbin,1000))
        if nticks == 0:
            nticks = 1
            tbin -= 1
        samples = numpy.random.normal(size=nticks)
        tr = pb.Trace(channel=chan, tbin=tbin)
        tr.samples.elements[:] = samples
        frame1.traces.append(tr)
        tot = numpy.sum(samples)
        if tot < 0:
            negs.append(ind)
            negs_tot.append(tot)
            brl = frame1.channel_masks["bad"].bin_range_lists[chan]
            brl.beg.append(tbin)
            brl.end.append(tbin+nticks)
    if len(negs) > 0:
        frame1.frame_tags.append("negged")
    t2 = time.time()
    print("frame build time: %f" %(t2-t1))

    tmpdir = tempfile.mkdtemp()
    hdf = h5py.File(os.path.join(tmpdir, "test_frame.hdf"), 'w')
    frompb.Frame(frame1, hdf)
    t3 = time.time()
    print("frame write time: %f" % (t3-t2))

    frame2 = topb.Frame(hdf, pb)
    t4 = time.time()
    print("frame read time: %f" % (t4-t3))
    assert_same_frames(frame1, frame2)
    shutil.rmtree(tmpdir)

def test_dense_frame():
    'Test dense frame conversion'

    t1 = time.time()
    frame1 = pb.Frame(ident=2, time=time.time(), tick=500.0)
    chanset = list(range(1000))
    negs = frame1.tagged_traces["negs"].elements
    negs_tot = frame1.trace_summaries["negs"].elements
    for ind in range(500):
        chan = ind
        chanset.remove(chan)
        tbin = 0
        nticks = 1000
        samples = numpy.random.normal(size=nticks)
        tr = pb.Trace(channel=chan, tbin=tbin)
        tr.samples.elements[:] = samples
        frame1.traces.append(tr)
        tot = numpy.sum(samples)
        if tot < 0:
            negs.append(ind)
            negs_tot.append(tot)
            brl = frame1.channel_masks["bad"].bin_range_lists[chan]
            brl.beg.append(tbin)
            brl.end.append(tbin+nticks)
    if len(negs) > 0:
        frame1.frame_tags.append("negged")
    t2 = time.time()
    print("frame build time: %f" %(t2-t1))

    tmpdir = tempfile.mkdtemp()
    hdf = h5py.File(os.path.join(tmpdir, "test_frame.hdf"), 'w')
    frompb.Frame(frame1, hdf)
    t3 = time.time()
    print("frame write time: %f" % (t3-t2))

    frame2 = topb.Frame(hdf, pb)
    t4 = time.time()
    print("frame read time: %f" % (t4-t3))
    assert_same_frames(frame1, frame2)
    shutil.rmtree(tmpdir)

def assert_same_frames(f1, f2):
    for field in "ident time tick".split():
        assert(getattr(f1, field) == getattr(f2, field))
    for t1,t2 in zip(f1.traces, f2.traces):
        assert(t1.channel == t2.channel)
        assert(t1.tbin    == t2.tbin)
        assert(len(t1.samples.elements) == len(t2.samples.elements))
        for s1,s2 in zip(t1.samples.elements, t2.samples.elements):
            assert(s1 == s2)
    for ft1,ft2 in zip(f1.frame_tags, f2.frame_tags):
        assert (ft1 == ft2)
    assert(len(f1.tagged_traces) == len(f2.tagged_traces))
    assert(len(f1.trace_summaries) == len(f2.trace_summaries))


if '__main__' == __name__:
    test_depo()
    test_sparse_frame()
    test_dense_frame()
    
    
