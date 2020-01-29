#!/usr/bin/env python3
'''
Converters from HDF to PB
'''

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
    
