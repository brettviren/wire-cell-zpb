#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/DepoSink.h"
#include "WireCellIface/SimpleDepo.h"
#include "WireCellUtil/Logging.h"
#include "zio/message.hpp"

#include <iostream>

using namespace WireCell;

using namespace std;

/// inproc leads to some timing problem not seen with ipc and tpc
const char* addr = "inproc://test_depozpb";
//const char* addr = "tcp://127.0.0.1:5678";
//const char* addr = "ipc://test_depozpb.ipc";

// a very very ugly server
void server(Log::logptr_t l)
{
    zio::context_t ctx;
    zio::socket_t s(ctx, ZMQ_SERVER);
    s.bind(addr);

    std::map<uint32_t,uint32_t> rids;
    std::vector<zio::message_t> tosend;

    zmq::poller_t<> poller;
    poller.add(s, zmq::event_flags::pollin);
    const auto wait = std::chrono::milliseconds{3000};

    int count = 0;
    while (true) {
        l->info("server: {} polling", count);
        std::vector<zmq::poller_event<>> events(1);
        int rc = poller.wait_all(events, wait);
        if (rc == 0) {
            l->info("server: timeout");
            break;
        }

        zio::message_t msg;
        l->info("server: recv");
        auto res = s.recv(msg);
        assert(res);
        uint32_t rid = msg.routing_id();
        assert (rid > 0);
        zio::Message MSG;
        MSG.decode(msg);

        l->info("server: {} recvd {}", count, MSG.prefix().dumps());
        ++count;

        if (rids.empty()) {
            rids[rid]=0;
            tosend.emplace_back(MSG.encode());
            continue;
        }
        if (rids.size() == 1) {
            uint32_t orid = rids.begin()->first;
            if (rid == orid) {  // more of the same
                tosend.emplace_back(MSG.encode());
                continue;
            }
            // now have 2
            rids[rid] = orid;
            rids[orid] = rid;
        }
        tosend.emplace_back(MSG.encode());

        for (auto& ts : tosend) {
            rid = ts.routing_id();
            uint32_t orid = rids[rid];
            ts.set_routing_id(orid);
            l->info("server: route {} -> {}", rid, orid);
            auto ses = s.send(ts, zio::send_flags::none);
            assert(ses);
        }
        tosend.clear();
    }

}

void depo_extraction(Log::logptr_t l)
{
    Zpb::DepoSink us;           // source of zio messages

    auto us_cfg = us.default_configuration();
    us_cfg["timeout"] = 1000;
    us_cfg["ports"]["extract-depos"]["stype"] = ZMQ_CLIENT;
    us_cfg["ports"]["extract-depos"]["connects"][0] = addr;

    l->info("DepoSink config:\n{}", us_cfg);
    l->info("configuring upstream");
    us.configure(us_cfg);

    usleep(100000);

    l->info("extract thread: sending depo");
    IDepo::pointer depo1 = std::make_shared<SimpleDepo>(6.9,WireCell::Point(0,0,0));
    bool keep_going = us(depo1);
    assert(keep_going);

    l->info("extract thread: sending eos");
    keep_going = us(nullptr);
    assert(keep_going);

    l->info("extract thread: sending 2nd eos as EOT");
    keep_going = us(nullptr);
    assert(!keep_going);
}

void depo_injection(Log::logptr_t l)
{
    Zpb::DepoSource ds;         // down stream sources IDepo consumes messages
    auto ds_cfg = ds.default_configuration();
    ds_cfg["timeout"] = 1000;
    ds_cfg["ports"]["inject-depos"]["stype"] = ZMQ_CLIENT;
    ds_cfg["ports"]["inject-depos"]["connects"][0] = addr;

    cerr << "DepoSource config:\n" << ds_cfg << "\n";

    l->info("configuring downstream");
    ds.configure(ds_cfg);

    usleep(100000);

    l->info("inject thread: recving depo");
    IDepo::pointer depo2 = nullptr;
    bool keep_going = ds(depo2);
    assert(keep_going);
    assert(depo2);

    IDepo::pointer depo3 = nullptr;
    l->info("inject thread: recving eos");
    keep_going = ds(depo3);
    assert(keep_going);
    assert(depo3 == nullptr);

    IDepo::pointer depo4 = nullptr;
    l->info("inject thread: recving EOT");
    keep_going = ds(depo4);
    assert(!keep_going);
    assert(depo4 == nullptr);
}

int main()
{
    Log::add_stderr(true, "debug");
    Log::set_level("debug", "zpb");
    Log::logptr_t l(Log::logger("tst"));
    l->info("test_depozpb starting");

    l->info("starting server thread");
    std::thread ser(server, l);

    l->info("starting client threads");
    std::thread tus(depo_extraction, l);
    std::thread tds(depo_injection, l);

    l->info("joining threads");
    tus.join();
    tds.join();
    ser.join();

    l->info("bye");
    return 0;
}
