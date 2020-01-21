#include "WireCellZpb/DepoSink.h"
#include "WireCellUtil/NamedFactory.h"


WIRECELL_FACTORY(ZpbDepoSink, WireCell::Zpb::DepoSink,
                 WireCell::IDepoSink, WireCell::IConfigurable)

using namespace WireCell;

Zpb::DepoSink::DepoSink()
    : FlowConverter<IDepo, wctzpb::Depo>("ZpbDepoSink","wct-zpb-depo-sink",
                                         "extract","extract-depos")
{
}

bool Zpb::DepoSink::operator()(const WireCell::IDepo::pointer& wctdat)
{
    return wct2zpb(wctdat);
}


#if 0

const std::string PORTNAME = "output";

Zpb::DepoSink::DepoSink()
    : NodeConfigurable(NodeConfigurable::node_config_t{
            "ZpbDepoSink",
            "wct-zpb-depo-sink",
            {{PORTNAME,ZMQ_CLIENT}}})
{
}

Zpb::DepoSink::~DepoSink()
{
}

void Zpb::DepoSink::user_default_configuration(WireCell::Configuration& cfg) const
{
    cfg["credits"] = 10;
}
void Zpb::DepoSink::user_configure(const WireCell::Configuration& cfg)
{
    int credits = cfg["credits"].asInt();
    m_flow = make_flow(PORTNAME, "extract", 10);
    if (!m_flow) {
        THROW(RuntimeError() << errmsg{"failed to make flow"});
    }
}
bool Zpb::DepoSink::operator()(const IDepo::pointer& depo)
{
    zio::Message msg("FLOW");        

    if (!depo) {
        if (m_had_eos) {
            m_flow->send_eot(msg);
            m_flow->recv_eot(msg, m_timeout);
            return false;
        }
        m_had_eos = true;
        bool ok = m_flow->put(msg);
        if (!ok) {
            zio::Message eot;
            m_flow->send_eot(eot);
            return false;
        }
        return true;
    }

    wctzpb::Depo out = Zpb::convert(depo);
    zio::message_t pl = pack(out);
    msg.add(std::move(pl));

    bool ok = m_flow->put(msg);
    if (!ok) {                  // got eot from remote
        zio::Message eot;
        m_flow->send_eot(eot);
        return false;
    }
    return true;
}



#endif
