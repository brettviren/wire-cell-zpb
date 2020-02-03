#!/usr/bin/env python3
'''
Converters from HDF to PB
'''
import numpy

def Depo(group, pbmod):
    '''Fill PB depo object with info from HDF group.

    This is inverse of @ref frompb.Depo()
    '''
    ds = group.get("depo")

    depo = pbmod.Depo()
    depo.pos.x = ds[0]
    depo.pos.y = ds[1]
    depo.pos.z = ds[2]
    depo.time = ds[3]
    depo.charge = ds[4]
    depo.energy = ds[5]
    depo.extent_long = ds[6]
    depo.extent_tran = ds[7]
    depo.ident = group.attrs["ident"]
    depo.trackid = group.attrs["trackid"]
    depo.pdg = group.attrs["pdg"]
    return depo
    

def Frame(group, pbmod):
    '''
    if dataset traces exists then its sparse
    if dataset samples exists then its dense
    '''
    frame = pbmod.Frame()
    attrs = dict(group.attrs)

    frame.ident = attrs.pop("ident")
    frame.time = attrs.pop("time")
    frame.tick = attrs.pop("tick")
    for k,v in attrs.items():
        if v == b'tag\0':
            frame.frame_tags.append(k)

    tgrp = group.get("tagged")
    for tag in tgrp:
        frame.tagged_traces[tag].elements.extend(tgrp.get(tag))

    sgrp = group.get("summaries")
    for tag in sgrp:
        frame.trace_summaries[tag].elements.extend(sgrp.get(tag))

    cgrp = group.get("cmm")
    for tag in cgrp:
        cms = frame.channel_masks[tag]
        ds = cgrp.get(tag)
        for chid, beg, end in ds:
            rl = cms.bin_range_lists[chid]
            rl.beg.append(beg)
            rl.end.append(end)

    if "samples" in group:      # dense
        print ("dense traces")
        chans = group["chans"]
        block = group["samples"]
        tbin = block.attrs["tbin"]
        for ind, channel in enumerate(chans):
            trace = pbmod.Trace(channel = channel, tbin = tbin)
            trace.samples.elements.extend(block[ind])
            frame.traces.append(trace)
    if "traces" in group:       # sparse
        print ("sparse traces")
        chans = group["chans"]
        tbins = group["tbins"]
        tg = group["traces"]
        for ind, channel in enumerate(chans):
            trace = pbmod.Trace(channel = channel, tbin = tbins[ind])
            # without numpy this get is slow
            hdtr = numpy.asarray(tg.get(str(ind)))
            trace.samples.elements[:] = hdtr
            frame.traces.append(trace)



    return frame
