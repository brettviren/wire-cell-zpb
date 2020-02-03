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
        for s in samples:
            tr.samples.elements.append(s)
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

    tmpdir = tempfile.mkdtemp()
    hdf = h5py.File(os.path.join(tmpdir, "test_frame.hdf"), 'w')
    frompb.Frame(frame1, hdf)
    frame2 = topb.Frame(hdf, pb)

    shutil.rmtree(tmpdir)

if '__main__' == __name__:
    #test_depo()
    test_sparse_frame()
    
    
