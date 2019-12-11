#include "WireCellZpb/DepoSource.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "input";

Zpb::DepoSource::DepoSource()
    : NodeConfigurable(NodeConfigurable::node_config_t{
            "ZpbDepoSource",
            "wct-zpb-depo-source",
            {{PORTNAME,ZMQ_SUB}}})
{
}

Zpb::DepoSource::~DepoSource()
{
}

bool Zpb::DepoSource::validate()
{
    m_port = m_node.port(PORTNAME);
    return m_port != nullptr;
}

bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    wirecell::zpb::data::Depo out;
    recv(m_port, out);

    return true;
}

