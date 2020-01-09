#include "DepoSource.h"
#include "Converters.h"
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

void Zpb::DepoSource::online()
{
    m_flow = make_flow(PORTNAME, "inject", 10);
    if (!m_flow) {
        THROW(RuntimeError() << errmsg{"failed to make flow"});
    }
}


bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    depo = nullptr;

    zio::Message msg;
    bool ok = m_flow->get(msg, m_timeout);
    if (!ok) {
        zio::Message eot;
        m_flow->send_eot(eot);
        return false;           // timeout, eot or other error
    }

    const zio::multipart_t& pls = msg.payload();
    if (!pls.size()) {           // EOS
        return true;
    }

    wctzpb::Depo out;
    unpack(pls[0], out);
    depo = Zpb::convert(out);

    return true;
}

