#include "WireCellZpb/Converters.h"

#include "WireCellZpb/DataImps.h"

using namespace WireCell;

IDepo::pointer Zpb::convert(const wctzpb::Depo& depo)
{
    return std::make_shared<Zpb::Depo>(depo);
}
                     
wctzpb::Depo Zpb::convert(const IDepo::pointer& depo)
{
    wctzpb::Depo out;
    out.set_ident((uint64_t)depo.get()); // not guaranteed unique
    auto* pos = out.mutable_pos();
    pos->set_x(depo->pos().x());
    pos->set_y(depo->pos().y());
    pos->set_z(depo->pos().z());
    out.set_time(depo->time());
    out.set_charge(depo->charge());
    out.set_energy(depo->energy());
    out.set_trackid(depo->id());
    out.set_pdg(depo->pdg());
    out.set_prior((uint64_t)depo->prior().get());
    out.set_extent_long(depo->extent_long());
    out.set_extent_tran(depo->extent_tran());    
    return out;
}


