#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/DepoSink.h"
#include "WireCellIface/SimpleDepo.h"
#include "WireCellUtil/Logging.h"
#include "zio/message.hpp"

#include <iostream>

using namespace WireCell;

using namespace std;

const char* addr = "inproc://test_depozpb";

// a very very ugly server
void server(Log::logptr_t l)
{
    zio::context_t ctx;
    zio::socket_t s(ctx, ZMQ_SERVER);
    s.bind(addr);

    std::map<uint32_t,uint32_t> rids;
    std::vector<zio::message_t> tosend;

    while (true) {
        zio::message_t msg;
        l->info("server: recv");
        auto res = s.recv(msg);
        assert(res);
        uint32_t rid = msg.routing_id();
        assert (rid > 0);
        zio::Message MSG;
        MSG.decode(msg);

        l->info("server: recvd {}", MSG.prefix().dumps());

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


    l->info("extract thread: sending depo");

    IDepo::pointer depo1 = std::make_shared<SimpleDepo>(6.9,WireCell::Point(0,0,0));
    bool ok = us(depo1);
    assert(ok);

    l->info("extract thread: sending eos");
    ok = us(nullptr);
    assert(ok);
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

    l->info("inject thread: recving depo");

    IDepo::pointer depo2 = nullptr;
    bool ok = ds(depo2);
    assert(ok);
    assert(depo2);

    IDepo::pointer depo3 = nullptr;
    l->info("inject thread: recving eos");
    ok = ds(depo3);
    assert(ok);
    assert(depo3 == nullptr);
}

int main()
{
    Log::add_stderr(true, "debug");
    Log::set_level("debug", "zpb");
    Log::logptr_t l(Log::logger("tst"));
    l->info("test_depozpb starting");

    l->info("starting server thread");
    std::thread ser(server, l);
    usleep(100000);

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
