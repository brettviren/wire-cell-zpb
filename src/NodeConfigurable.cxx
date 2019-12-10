#include "WireCellZpb/NodeConfigurable.h"
#include "WireCellUtil/Exceptions.h"

using namespace WireCell;

Zpb::NodeConfigurable::NodeConfigurable(const std::string& wct_type,
                                        const std::string& default_name,
                                        zio::level::MessageLevel default_level)
    : m_node(default_name, (uint64_t)this)
    , m_wct_type(wct_type)
    , m_level(default_level)
    , l(Log::logger("zpb"))
{
}

Zpb::NodeConfigurable::~NodeConfigurable()
{
    m_node.offline();
}

WireCell::Configuration Zpb::NodeConfigurable::default_configuration() const
{
    Configuration cfg;
    cfg["node"] = m_node.nick();
    cfg["origin"] = (Json::UInt64) m_node.origin();
    cfg["ports"] = Json::objectValue;
    // {name: "portname",
    //   binds: ["", "tcp://127.0.0.1:5678"], // examples, can omit both binds+connects
    //   connects: ["tcp://127.0.0.1:5678", {nodename:"nodenick",portname:"spigot"}]
    //   stype: "pub" (or "PUB")
    //  },...]
    cfg["level"] = (int)m_level;
    // any extra headers to advertise.
    cfg["headers"] = Json::objectValue;
    return cfg;
}


void Zpb::NodeConfigurable::configure(const WireCell::Configuration& cfg)
{
    m_node.set_nick(get<std::string>(cfg, "node", m_node.nick()));
    m_node.set_origin(get<Json::UInt64>(cfg, "origin", (Json::UInt64)m_node.origin()));
    m_level = (zio::level::MessageLevel)get<int>(cfg, "level", (int)m_level);

    const std::map<std::string, int> zstmap = {
        {"pub",ZMQ_PUB},
        {"sub",ZMQ_SUB},
        {"push",ZMQ_PUSH},
        {"pull",ZMQ_PULL},
    };

    if (cfg["ports"].empty()) {
        l->warn("node {}: no ports defined");
    }
    for (auto name : cfg["ports"].getMemberNames()) {
        auto body = cfg["ports"][name];

        if (name.empty()) {
            l->warn("node {}: request for unnamed port", m_node.nick());
        }

        std::string tmp = get<std::string>(body, "stype", "pub");
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        auto maybe = zstmap.find(tmp);
        if (maybe == zstmap.end()) {
            l->critical("node {}: unknown socket type: {}", m_node.nick(), tmp);
            THROW(ValueError() << errmsg{"unknown socket type"});
        }
        int stype = maybe->second;

        auto port = m_node.port(name, stype);
        
        auto binds = body["binds"];
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
        auto connects = body["connects"];
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

    m_headers = {{"WCT-Type",m_wct_type}};
    for (auto key : cfg["headers"].getMemberNames()) {
        auto val = cfg["headers"][key];
        m_headers.push_back(zio::header_t(key,val.asString()));
    }


    online();
}


void Zpb::NodeConfigurable::online()
{
    // subclass may override this method, and then probably should
    // call it explicitly.  Override to do any implementation-specific
    // sanity checking on the m_node.
    m_node.online(m_headers);
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

    port->send(m_level, convert.format(), convert(pl),
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
