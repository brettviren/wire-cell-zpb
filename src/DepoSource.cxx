#include "WireCellZpb/DepoSource.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

Zpb::DepoSource::DepoSource()
    : NodeConfigurable("ZpbDepoSource", "wct-zpb-deposource")
{
}

Zpb::DepoSource::~DepoSource()
{
}

void Zpb::DepoSource::online()
{
    const auto& portnames = m_node.portnames();
    if (portnames.size() != 1) {
        l->critical("{}: unexpected number of ports: {}",
                    m_node.nick(), portnames.size());
        THROW(ValueError() << errmsg{"unexpected number of ports"});
    }
    m_port = m_node.port(portnames[0]);
    this->NodeConfigurable::online();
}

bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    wirecell::zpb::data::Depo out;
    recv(m_port, out);

    return true;
}

