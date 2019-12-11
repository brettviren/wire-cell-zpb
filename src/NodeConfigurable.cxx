#include "WireCellZpb/NodeConfigurable.h"
#include "WireCellUtil/Exceptions.h"

using namespace WireCell;

Zpb::NodeConfigurable::NodeConfigurable(const node_config_t& nc)
    : m_node(nc.nick)
    , m_nc(nc)
    , l(Log::logger("zpb"))
{
    if (!m_nc.origin) {
        m_nc.origin = (uint64_t)this;
        m_node.set_origin(m_nc.origin);
    }
}

Zpb::NodeConfigurable::~NodeConfigurable()
{
    m_node.offline();
}

static
WireCell::Configuration jsonify(const Zpb::NodeConfigurable::node_config_t& nc)
{
    /// JsonCPP doesn't support initializer lists so we exaustively make this....
    Configuration cfg;
    cfg["nick"] = nc.nick;
    cfg["origin"] = (Json::UInt64)nc.origin;
    cfg["level"] = (int)nc.level;
    cfg["ports"] = Json::objectValue;
    for (const auto& pp : nc.ports) {
        cfg["ports"][pp.first]["stype"] = pp.second;
    }
    cfg["headers"] = Json::objectValue; // keys may be duplicate
    for (const auto& hh : nc.headers) {
        Configuration jhh = Json::arrayValue;
        jhh[0] = hh.first;
        jhh[1] = hh.second;
        cfg["headers"].append(jhh);
    }
    return cfg;
}

WireCell::Configuration Zpb::NodeConfigurable::default_configuration() const
{
    return jsonify(m_nc);
}

static int compatible_stype(int def, const Configuration& cfg)
{
    if (cfg.empty()) return def;
    const int want = cfg.asInt();
    if (def == want) { return want; }
    if ((def  == ZMQ_PUB or def  == ZMQ_PUSH) and
        (want == ZMQ_PUB or want == ZMQ_PUSH)) {
        return want;
    }
    if ((def  == ZMQ_SUB or def  == ZMQ_PULL) and
        (want == ZMQ_SUB or want == ZMQ_PULL)) {
        return want;
    }
    return -1;
}

void Zpb::NodeConfigurable::configure(const WireCell::Configuration& cfg)
{
    m_nc.nick = get<std::string>(cfg, "node", m_nc.nick);
    m_node.set_nick(m_nc.nick);
    m_nc.origin = get<Json::UInt64>(cfg, "origin", (Json::UInt64)m_nc.origin);
    m_node.set_origin(m_nc.origin);
    m_nc.level = (zio::level::MessageLevel)get<int>(cfg, "level", (int)m_nc.level);

    for (auto pp : m_nc.ports) {
        const std::string pname = pp.first;
        auto jport = cfg["ports"][pname];
        if (jport.empty()) {
            l->critical("node {}: no configuration for port {}", m_nc.nick, pname);
            THROW(ValueError() << errmsg{"missing port configuration"});
        }
        int stype = compatible_stype(pp.second, jport["stype"]);
        if (stype < 0) {
            l->critical("node {}: bad socket type for port {}", m_nc.nick, pname);
            THROW(ValueError() << errmsg{"bad socket type"});
        }

        auto port = m_node.port(pname, stype);

        auto binds = jport["binds"];
        if (!binds.empty()) {
            for (auto bind : binds) {
                if (bind.isNull() or bind.size() == 0) {
                    port->bind();
                    continue;
                }
                if (bind.isString()) {
                    port->bind(bind.asString());
                    continue;
                }
                if (bind.isObject()) {
                    port->bind(bind["host"].asString(), bind["tcpportnum"].asInt());
                    continue;
                }
            }
        }
        auto connects = jport["connects"];
        if (!connects.empty()) {
            for (auto conn : connects) {
                if (conn.isString()) {
                    port->connect(conn.asString());
                    continue;
                }
                if (conn.isObject()) {
                    port->connect(conn["nodename"].asString(), conn["portname"].asString());
                    continue;
                }
            }
        }
        if (binds.empty() and connects.empty()) {
            port->bind();       // default: ephemeral bind
        }

    }

    m_nc.headers.push_back(zio::header_t("WCT-Type",m_nc.wct_type));
    for (auto key : cfg["headers"].getMemberNames()) {
        auto val = cfg["headers"][key];
        m_nc.headers.push_back(zio::header_t(key,val.asString()));
    }


    online();
}



void Zpb::NodeConfigurable::online()
{
    bool ok = validate();
    if (!ok) {
        l->critical("node {}: validation failed", m_nc.nick);
        THROW(ValueError() << errmsg{"validation failed"});
    }
    m_node.online(m_nc.headers);
}

bool Zpb::NodeConfigurable::send_eos(zio::portptr_t port)
{
    wirecell::zpb::data::Payload pl;
    send(port, pl);
    return true;
}

bool Zpb::NodeConfigurable::send(zio::portptr_t port, ::google::protobuf::Message& msg)
{
    zio::converter::pbuf_t<wirecell::zpb::data::Payload> convert;
    wirecell::zpb::data::Payload pl;
    auto any = pl.add_objects();
    any->PackFrom(msg);

    port->send(m_nc.level, convert.format(), convert(pl),
               msg.GetTypeName());
    return true;
}
bool Zpb::NodeConfigurable::recv(zio::portptr_t port, ::google::protobuf::Message& msg, bool& is_eos)
{
    is_eos = false;
    zio::Header hdr;
    zio::byte_array_t buf;

    int rc = port->recv(hdr, buf);
    if (rc != 0) {
        l->error("{}: receive error {}", m_node.nick(), rc);
        return false;
    }
    if (hdr.format != "PBUF") {
        l->error("{}: receive unwanted type: {} (want PBUF)", m_node.nick(), hdr.format);
        return false;
    }
    if (hdr.label != msg.GetTypeName()) {
        l->error("{}: receive unwanted type: {} (want {})",
                 m_node.nick(), hdr.label, msg.GetTypeName());
    }
    zio::converter::pbuf_t<wirecell::zpb::data::Payload> convert;
    wirecell::zpb::data::Payload pl = convert(buf);
    auto npls = pl.objects_size();
    if (!npls) {
        is_eos = true;
        return true;
    }

    if (npls > 1) {
        l->warn("{}: too many payloads: {} (want 1)", m_node.nick(), npls);
    }
    pl.objects(0).UnpackTo(&msg);
    return true;
}


bool Zpb::NodeConfigurable::selftest()
{
    return true;
}