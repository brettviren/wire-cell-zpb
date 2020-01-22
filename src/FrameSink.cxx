#include "WireCellZpb/FrameSink.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"


WIRECELL_FACTORY(ZpbFrameSink, WireCell::Zpb::FrameSink,
                 WireCell::IFrameSink, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "output";

Zpb::FrameSink::FrameSink()
    : NodeConfigurable(NodeConfigurable::node_config_t{
            "ZpbFrameSink",
            "wct-zpb-frame-sink",
            {{PORTNAME,ZMQ_CLIENT}}})
{
}

Zpb::FrameSink::~FrameSink()
{
}

// fixme: all "sinks" in this package could probably inherit from the
// same intermediate class as this and the next config method will be identical.
void Zpb::FrameSink::user_default_configuration(WireCell::Configuration& cfg) const
{
    cfg["credit"] = 10;
}
void Zpb::FrameSink::user_configure(const WireCell::Configuration& cfg)
{
    int credit = cfg["credit"].asInt();
    m_flow = make_flow(PORTNAME);//, "extract", 10);
    if (!m_flow) {
        THROW(RuntimeError() << errmsg{"failed to make flow"});
    }
}
// fixme: this method is almost identical with the one for depos
// except for types.  It could be templated.
bool Zpb::FrameSink::operator()(const IFrame::pointer& frame)
{
    zio::Message msg("FLOW");        

    if (!frame) {
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

    wctzpb::Frame out = Zpb::convert(frame);
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



