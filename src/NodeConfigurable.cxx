#include "NodeConfigurable.h"
#include "WireCellUtil/Exceptions.h"

#include "wctzpb.pb.h"


using namespace WireCell;

Zpb::NodeConfigurable::NodeConfigurable(const node_config_t& nc)
    : m_node(nc.nick, nc.origin)
    , m_nc(nc)
    , l(Log::logger("zpb"))
    , m_timeout(100)
{
    if (!m_nc.origin) {
        m_nc.origin = (zio::origin_t)this;
    }
    m_node.set_origin(m_nc.origin);
}

Zpb::NodeConfigurable::~NodeConfigurable()
{
    l->debug("{}: destructor", m_nc.nick);
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
    cfg["headers"] = Json::objectValue;
    for (const auto& hh : nc.headers) {
        cfg["headers"][hh.first] = hh.second;
    }

    return cfg;
}

WireCell::Configuration Zpb::NodeConfigurable::default_configuration() const
{
    Configuration cfg = jsonify(m_nc);
    cfg["timeout"] = m_timeout; // recv timeout, msec
    return cfg;
}

static int compatible_stype(int def, const Configuration& cfg)
{
    // this is a rather hand-wringing way to use the default socket
    // type to hint what socket types are allowed to be provided by
    // configuration.

    if (cfg.empty()) return def;
    const int got = cfg.asInt();
    if (def == got) { return got; }

    bool def_sender = def == ZMQ_PUB or def == ZMQ_PUSH or def == ZMQ_RADIO or def == ZMQ_CLIENT;
    bool def_recver = def == ZMQ_SUB or def == ZMQ_PULL or def == ZMQ_DISH  or def == ZMQ_SERVER;

    bool got_sender = got == ZMQ_PUB or got == ZMQ_PUSH or got == ZMQ_RADIO or got == ZMQ_CLIENT;
    bool got_recver = got == ZMQ_SUB or got == ZMQ_PULL or got == ZMQ_DISH  or got == ZMQ_SERVER;

    if (def_sender and got_sender) {
        return got;
    }
    if (def_recver and got_recver) {
        return got;
    }
    return -1;
}

void Zpb::NodeConfigurable::configure(const WireCell::Configuration& cfg)
{
    m_nc.nick = get<std::string>(cfg, "nick", m_nc.nick);
    m_node.set_nick(m_nc.nick);
    m_nc.origin = get<Json::UInt64>(cfg, "origin", (Json::UInt64)m_nc.origin);
    m_node.set_origin(m_nc.origin);
    m_nc.level = (zio::level::MessageLevel)get<int>(cfg, "level", (int)m_nc.level);
    l->debug("node {}: origin: {}", m_nc.nick, m_nc.origin);
    m_timeout = get<int>(cfg, "timeout", m_timeout);


    int verbose = 0;
    if (! cfg["verbose"].empty()) {
        verbose = cfg["verbose"].asInt();
        if (verbose) {
            l->debug("node {}: verbose", m_nc.nick);
            zsys_init();
            m_node.set_verbose(verbose);
        }
    }

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

        l->info("node {}: make port {} type {}", m_nc.nick, pname, stype);
        auto port = m_node.port(pname, stype);

        auto binds = jport["binds"];
        auto connects = jport["connects"];
        if (binds.empty() and connects.empty()) {
            l->info("node {}: binding {} to ephemeral", m_nc.nick, pname);
            port->bind();       // default: ephemeral bind
        }
        else {
            l->debug("node {}: binds: {}", m_nc.nick, binds);
            for (auto bind : binds) {
                if (bind.empty()) {
                    l->info("node {}: bind to ephemeral", m_nc.nick);
                    port->bind();
                    continue;
                }
                if (bind.isString()) {
                    l->info("node {}: bind to {}", m_nc.nick, bind);
                    port->bind(bind.asString());
                    continue;
                }
                if (bind.isObject()) {
                    l->info("node {}: bind to {}", m_nc.nick, bind);
                    port->bind(bind["host"].asString(), bind["tcpportnum"].asInt());
                    continue;
                }
            }
            l->debug("node {}: connects: {}", m_nc.nick, connects);
            for (auto conn : connects) {
                if (conn.isString()) {
                    l->info("node {}: connect to {}", m_nc.nick, conn);
                    port->connect(conn.asString());
                    continue;
                }
                if (conn.isObject()) {
                    l->info("node {}: connect to {}", m_nc.nick, conn);
                    port->connect(conn["nodename"].asString(), conn["portname"].asString());
                    continue;
                }
            }
        }
    }

    m_nc.headers["WCT-Type"] = m_nc.wct_type;
    for (auto key : cfg["headers"].getMemberNames()) {
        auto val = cfg["headers"][key];
        m_nc.headers[key] = val.asString();
    }


    go_online();
    online();
}



void Zpb::NodeConfigurable::online()
{
}

void Zpb::NodeConfigurable::go_online()
{
    bool ok = validate();
    if (!ok) {
        l->critical("node {}: validation failed", m_nc.nick);
        THROW(ValueError() << errmsg{"validation failed"});
    }
    l->debug("{}: going online with extra headers:", m_node.nick());
    for (const auto& hh : m_nc.headers) {
        l->debug("\t{} = {}", hh.first, hh.second);
    }
    m_node.online(m_nc.headers);
}


zio::message_t Zpb::NodeConfigurable::pack(::google::protobuf::Message& msg)
{
    wctzpb::Payload pl;
    auto any = pl.add_objects();
    any->PackFrom(msg);

    zio::message_t ret(any->ByteSize());
    any->SerializeToArray(ret.data(), ret.size());
    return ret;
}

void Zpb::NodeConfigurable::unpack(const zio::message_t& spmsg, ::google::protobuf::Message& pbmsg)
{
    wctzpb::Payload pbpl;
    pbpl.ParseFromArray(spmsg.data(), spmsg.size());
    pbpl.objects(0).UnpackTo(&pbmsg);
}



Zpb::NodeConfigurable::flowptr_t
Zpb::NodeConfigurable::make_flow(const std::string& portname,
                                 const std::string& direction,
                                 int credits)
{

    auto port = m_node.port(portname);
    if (!port) return nullptr;

    auto flow = std::make_unique<zio::flow::Flow>(port);
    zio::Message msg("FLOW");
    zio::json fobj = {{"flow","BOT"},
                      {"direction",direction},
                      {"credits",credits}};
    msg.set_label(fobj.dump());

    int stype = zio::sock_type(port->socket());
    if (stype == ZMQ_SERVER) {
        // This strains the flow protocol a bit to pretend to be a
        // server.  It'll fall over if multiple clients try to
        // connect.  But it allows some testing with a simpler graph.
        bool ok = flow->recv_bot(msg, m_timeout);
        if (!ok) { return nullptr; }
        flow->send_bot(msg);
    }
    else {
        flow->send_bot(msg);
        bool ok = flow->recv_bot(msg, m_timeout);
        if (!ok) { return nullptr; }
    }
    return flow;
}


