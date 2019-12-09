#include "WireCellZpb/DepoSink.h"

#include "google/protobuf/message.h"
// fixme: this needs to go in a more global spot to at least be shared
// by other zpb classes, if not in zio!
namespace zio {
    struct PBUF : public Format {
        virtual const char* type() const { return "PBUF"; }
        byte_array_t buffer;
        size_t size() const { return buffer.size(); }
        const uint8_t* data() const { return buffer.data(); }

        PBUF(const ::google::protobuf::Message& msg) {
            size_t siz = msg.ByteSize();
            buffer.resize(siz);
            msg.SerializeToArray(buffer.data(), siz);
        }
        bool operator()(::google::protobuf::Message& msg) {
            return msg.ParseFromArray(buffer.data(), buffer.size());
        }
    };
}

#include "wctzpb.pb.h"

using namespace WireCell;

Zpb::DepoSink::DepoSink()
    : m_node("wct-zpb-deposink")
    , m_level(zio::level::info)
{
}

Zpb::DepoSink::~DepoSink()
{
}


WireCell::Configuration Zpb::DepoSink::default_configuration() const
{
    Configuration cfg;
    cfg["node"] = "wct-zpb-deposink";
    cfg["port"] = "depos";
    cfg["origin"] = (Json::Value::UInt64)this;
    cfg["stype"] = "pub";
    cfg["level"] = (int)m_level;
    return cfg;
}


void Zpb::DepoSink::configure(const WireCell::Configuration& cfg)
{
    m_node.set_nick(get<std::string>(cfg, "node", "wct-zpb-deposink"));
    uint64_t origin = get<uint64_t>(cfg, "origin", (Json::Value::UInt64)this);
    m_node.set_origin(origin);
    int stype = ZMQ_PUSH;
    std::string tmp = get<std::string>(cfg, "stype", "pub");
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    if (tmp == "pub") {
        stype = ZMQ_PUB;
    }
    m_port = m_node.port(get<std::string>(cfg, "port", "depos"), stype);
    m_level = (zio::level::MessageLevel)get<int>(cfg, "level", (int)m_level);

    m_node.online();
}


bool Zpb::DepoSink::operator()(const IDepo::pointer& depo)
{
    wirecell::zpb::data::Payload pl;
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

    zio::PBUF pbuf(out);
    m_port->send(m_level, pbuf, "wirecell.zpb.data.Depo");
}


