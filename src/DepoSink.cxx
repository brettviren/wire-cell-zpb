#include "WireCellZpb/DepoSink.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSink, WireCell::Zpb::DepoSink,
                 WireCell::IDepoSink, WireCell::IConfigurable)



using namespace WireCell;

const std::string PORTNAME = "output";

Zpb::DepoSink::DepoSink()
    : NodeConfigurable(NodeConfigurable::node_config_t{
            "ZpbDepoSink",
            "wct-zpb-depo-sink",
            {{PORTNAME,ZMQ_PUB}}})
{
}

Zpb::DepoSink::~DepoSink()
{
}

WireCell::Configuration Zpb::DepoSink::default_configuration() const
{

}

bool Zpb::DepoSink::validate()
{
    // cache the port pointer to avoid a lookup each call
    m_port = m_node.port(PORTNAME);
    return m_port != nullptr;
}

bool Zpb::DepoSink::operator()(const IDepo::pointer& depo)
{
    if (!depo) {
        return send_eos(m_port);
    }

    wirecell::zpb::data::Depo out;

    out.set_ident((uint64_t)depo.get());
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

    return send(m_port, out);
}



