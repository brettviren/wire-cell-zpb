#include "WireCellZpb/DepoSink.h"
#include "WireCellUtil/NamedFactory.h"

#include "wctzpb.pb.h"

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

bool Zpb::DepoSink::validate()
{
    auto port = m_node.port(PORTNAME);
    if (!port) return false;

    m_flow = std::make_unique<zio::flow::Flow>(port);
    zio::Message msg("FLOW");
    zio::json fobj = {{"flow","BOT"},{"direction","extract"},{"credits",10}};
    msg.set_label(fobj.dump());

    // WARNING, this send/recv will deadlock if our other end is
    // running in the same thread.
    m_flow->send_bot(msg);
    return m_flow->recv_bot(msg, m_timeout);
}

bool Zpb::DepoSink::operator()(const IDepo::pointer& depo)
{
    zio::Message msg("FLOW");        

    if (!depo) {
        if (m_had_eos) {
            m_flow->eot(msg);   // wait for answer
            return false;
        }
        m_had_eos = true;
        m_flow->put(msg);       // empty payload implies WCT EOS
        return true;
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
    zio::message_t pl = pack(out);
    msg.add(std::move(pl));

    bool ok = m_flow->put(msg);
    if (!ok) {                  // got eot from remote
        zio::Message eot("FLOW");
        m_flow->eot(eot,0);     // ack, but don't wait
        return false;           // don't call us any more
    }
    return true;
}



