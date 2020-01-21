#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "input";

Zpb::DepoSource::DepoSource()
    : FlowConverter("ZpbDepoSource", "wct-zpb-depo-source",
                    "inject", "inject-depos")
{
}


bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    return pb2wct(depo);
}

#if 0

Zpb::DepoSource::~DepoSource()
{
}

void Zpb::DepoSource::user_default_configuration(WireCell::Configuration& cfg) const
{
    cfg["credits"] = 10;
}

void Zpb::DepoSource::user_configure(const WireCell::Configuration& cfg)
{
    int credits = cfg["credits"].asInt();
    m_flow = make_flow(PORTNAME, "inject", credits);
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

#endif
