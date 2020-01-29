#include "WireCellUtil/Exceptions.h"
#include "WireCellZpb/FlowConfigurable.h"

#include <memory>

using namespace WireCell;

Zpb::FlowConfigurable::FlowConfigurable(const std::string& direction,
                                        const std::string& nodename)
    : m_direction(direction)
    , m_node(nodename, (zio::origin_t)this)
    , l(Log::logger("zpb"))
{
}

Zpb::FlowConfigurable::~FlowConfigurable()
{
    l->info("FlowConfigurable destroying node");
    m_node.offline();
}

WireCell::Configuration Zpb::FlowConfigurable::default_configuration() const
{
    Configuration cfg;
    // zmq
    cfg["timeout"] = m_timeout;
    cfg["binds"] = Json::arrayValue;
    cfg["connects"] = Json::arrayValue;
    // zyre
    cfg["nodename"] = m_node.nick();
    cfg["headers"] = Json::objectValue;
    // zio
    cfg["portname"] = m_portname;
    // msg
    cfg["origin"] = (Json::UInt64)m_node.origin();
    cfg["level"] = (int)m_level;
    // flow
    cfg["credit"] = m_credit;
    // zdb
    cfg["attributes"] = Json::objectValue;

    user_default_configuration(cfg);
    return cfg;
}


void Zpb::FlowConfigurable::configure(const WireCell::Configuration& cfg)
{
    m_timeout = get<int>(cfg, "timeout", m_timeout);
    const std::string nick = get<std::string>(cfg, "nodename", m_node.nick());
    m_node.set_nick(nick);
    m_node.set_origin(get(cfg, "origin",  (Json::UInt64)m_node.origin()));
    m_portname = get<std::string>(cfg, "portname", m_portname);
    m_credit = get<int>(cfg, "credit", m_credit);
    m_level = (zio::level::MessageLevel)get<int>(cfg, "level", (int)m_level);
    m_stype = get<int>(cfg, "stype", m_stype);
    if (! (m_stype == ZMQ_CLIENT or m_stype == ZMQ_SERVER)) {
        l->error("node {}: may only use CLIENT or SERVER stype, got {}",
                 nick, m_stype);
        THROW(RuntimeError() << errmsg{"unsupported socket type"});
    }

    int verbose = 0;
    if (! cfg["verbose"].empty()) {
        verbose = cfg["verbose"].asInt();
        if (verbose) {
            l->debug("node {}: verbose", nick);
            zsys_init();
            m_node.set_verbose(verbose);
        }
    }

    auto port = m_node.port(m_portname, m_stype);

    auto binds = cfg["binds"];
    auto connects = cfg["connects"];
    if (binds.empty() and connects.empty()) {
        l->info("node {}: binding {} to ephemeral", nick, m_portname);
        port->bind();       // default: ephemeral bind
    }
    else {
        for (auto bind : binds) {
            l->debug("node {}: bind: {}", nick, bind);
            if (bind.empty()) {
                l->info("node {}: bind to ephemeral", nick);
                port->bind();
                continue;
            }
            if (bind.isString()) {
                l->info("node {}: bind to {}", nick, bind);
                port->bind(bind.asString());
                continue;
            }
            if (bind.isObject()) {
                l->info("node {}: bind to {}", nick, bind);
                port->bind(bind["host"].asString(), bind["tcpportnum"].asInt());
                continue;
            }
            l->error("node {}: unknown bind type {}", nick, bind);
            THROW(RuntimeError() << errmsg{"unknown bind type"});
        }
        for (auto conn : connects) {
            l->debug("node {}: connect: {}", nick, conn);
            if (conn.isString()) {
                port->connect(conn.asString());
                continue;
            }
            if (conn.isObject()) {
                port->connect(conn["nodename"].asString(), conn["portname"].asString());
                continue;
            }
            l->error("node {}: unknown connect type {}", nick, conn);
            THROW(RuntimeError() << errmsg{"unknown connect type"});
        }
    }

    m_headers["ZPB-Type"] = m_type_name;
    for (auto key : cfg["headers"].getMemberNames()) {
        auto val = cfg["headers"][key];
        m_headers[key] = val.asString();
    }

    zio::json fobj = {
        {"flow", "BOT"},
        {"direction",m_direction},
        {"credit",m_credit},
        {"dtype", m_type_name}
    };
    for (auto key : cfg["attributes"].getMemberNames()) {
        auto val = cfg["attributes"][key];
        fobj[key] = val.asString();
    }
    m_bot_label = fobj.dump();

    m_flow = std::make_unique<zio::flow::Flow>(port);
    if (!m_flow) {
        THROW(RuntimeError() << errmsg{"failed to make flow"});
    }

    bool ok = user_configure(cfg);
    if (!ok) {
        l->critical("node {}: configuration failed", nick);
        THROW(ValueError() << errmsg{"configuration failed"});
    }

    l->debug("{}: going online with headers:", nick);
    for (const auto& hh : m_headers) {
        l->debug("\t{} = {}", hh.first, hh.second);
    }
    m_node.online(m_headers);

    ok = user_online();
    if (!ok) {
        l->critical("node {}: failed to go online", nick);
        THROW(ValueError() << errmsg{"online failed"});
    }
}



bool Zpb::FlowConfigurable::pre_flow()
{
    if (m_did_bot) {
        return true;
    }
    m_did_bot = true;           // call once

    const std::string nick = m_node.nick();

    zio::Message msg("FLOW");
    msg.set_label(m_bot_label);

    if (m_stype == ZMQ_SERVER) {
        // This strains the flow protocol a bit to pretend to be a
        // server.  It'll fall over if multiple clients try to
        // connect.  But it allows some testing with a simpler graph.
        l->debug("node {}: serverish BOT recv", nick);
        bool ok = m_flow->recv_bot(msg, m_timeout);
        if (!ok) {
            l->warn("node {}: serverish BOT recv timeout ({})",
                    nick, m_timeout);
            m_flow = nullptr;
            return false;
        }
        l->debug("node {}: serverish BOT send", nick);
        m_flow->send_bot(msg);
    }
    else {                      // client
        l->debug("node {}: clientish BOT send", nick);
        m_flow->send_bot(msg);
        l->debug("node {}: clientish BOT recv", nick);
        bool ok = m_flow->recv_bot(msg, m_timeout);
        if (!ok) {
            l->warn("node {}: clientish BOT recv timeout ({})",
                    nick, m_timeout);
            m_flow = nullptr;
            return false;
        }
    }

    if (m_direction == "extract") {
        l->debug("node {}: slurp pay for {}", nick, m_direction);
        m_flow->slurp_pay(0);
    }
    else {                      // inject
        l->debug("node {}: flush pay for {}", nick, m_direction);
        m_flow->flush_pay();
    }

    return true;
}

zio::message_t Zpb::FlowConfigurable::pack(::google::protobuf::Message& msg)
{
    wctzpb::Payload pl;
    auto any = pl.add_objects();
    any->PackFrom(msg);

    zio::message_t ret(any->ByteSize());
    any->SerializeToArray(ret.data(), ret.size());
    return ret;
}

void Zpb::FlowConfigurable::unpack(const zio::message_t& spmsg,
                                   ::google::protobuf::Message& pbmsg)
{
    wctzpb::Payload pbpl;
    pbpl.ParseFromArray(spmsg.data(), spmsg.size());
    pbpl.objects(0).UnpackTo(&pbmsg);
}


void Zpb::FlowConfigurable::finalize()
{
    const std::string nick = m_node.nick();
    l->debug("node {}: FINALIZE", nick);
    if (m_flow) {
        zio::Message msg;
        l->debug("node {}: send EOT", nick);
        m_flow->send_eot(msg);
        l->debug("node {}: recv EOT", nick);
        m_flow->recv_eot(msg, m_timeout);
    }
    m_node.offline();
    m_flow = nullptr;
}
