#include "WireCellUtil/Logging.h"

#include "zio/message.hpp"
#include <unistd.h>
#include <chrono>

using namespace WireCell;

//const char* addr = "inproc://test_depozpb";
const char* addr = "tcp://127.0.0.1:5678";

typedef std::chrono::duration<int64_t,std::micro> microseconds_type;

// a very very ugly server
static
void server(Log::logptr_t l)
{
    zmq::context_t ctx;
    zmq::socket_t s(ctx, ZMQ_SERVER);
    s.bind(addr);

    std::map<uint32_t,uint32_t> rids;
    std::map<uint32_t, std::vector<int> > tosend;

    zmq::poller_t<> poller;
    poller.add(s, zmq::event_flags::pollin);
    l->info("server: loop starts");
    const auto wait = std::chrono::milliseconds{-1};
    
    int64_t ttot=0, tmin=0, tmax=0;
    int count = 0;

    int dead = 0;

    while (true) {
        auto t0 = std::chrono::high_resolution_clock::now();
        std::vector<zmq::poller_event<>> events(1);
        try {
            int rc = poller.wait_all(events, wait);
            if (rc == 0) {
                l->info("server: wait returns {}", rc);
                break;
            }
        } catch (zmq::error_t e) {
            l->info("server: poller exception: {}", e.what());
            return;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        const microseconds_type dtus = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
        const int64_t dt = dtus.count();
        
        if (count == 0) {
            tmin = tmax = dt;
        }
        else {
            tmin = std::min(tmin, dt);
            tmax = std::max(tmax, dt);
        }
        count += 1;
        ttot += dt;
        l->info("server: #{} [{}, {}] <{}> tot={} dt={} [us]", count,
                tmin, tmax, (ttot/count), ttot, dt);
            

        zmq::message_t msg;
        auto res = s.recv(msg);
        assert(res);
        uint32_t rid = msg.routing_id();
        assert (rid > 0);

        if (msg.size() == 0) {
            if (dead) {
                break;          // bail after more than 1
            }
            dead += 1;
        }

        // the "message"
        int them = *static_cast<int*>(msg.data());

        l->debug("server: recvd {} {}", rid, them);

        if (rids.empty()) {
            rids[rid]=0;
            tosend[rid].push_back(them);
            continue;
        }
        if (rids.size() == 1) {
            uint32_t orid = rids.begin()->first;
            if (rid == orid) {  // more of the same
                tosend[rid].push_back(them);
                continue;
            }
            // now have 2
            rids[rid] = orid;
            rids[orid] = rid;
        }
        tosend[rid].push_back(them);

        for (const auto& rid_v : tosend) {
            uint32_t rid = rid_v.first;
            for (auto them : rid_v.second) {
                uint32_t orid = rids[rid];
                zmq::message_t msg(&them, sizeof(int));        
                msg.set_routing_id(orid);
                auto ses = s.send(msg, zmq::send_flags::none);
                assert(ses);
            }
        }
        tosend.clear();
    }
}

static
void client(Log::logptr_t l, int me)
{
    zmq::context_t ctx;
    zmq::socket_t c(ctx, ZMQ_CLIENT);
    c.connect(addr);
    //usleep(100000);
    for (int count=0; count<2000; ++count) {
        zmq::message_t msg(&me, sizeof(int));        
        l->debug("client {} send", me);
        c.send(msg, zmq::send_flags::none);
        auto res = c.recv(msg);
        assert(res);
        int them = *static_cast<int*>(msg.data());
        l->debug("client {} got {}", me, them);
    }
    c.send(zmq::message_t(), zmq::send_flags::none);
}
int main()
{
    Log::add_stderr(true, "debug");
    Log::set_level("debug", "zpb");
    Log::set_level("info", "tst");
    Log::logptr_t l(Log::logger("tst"));
    l->info("test_tcs starting");

    std::thread ser(server, l);
    //usleep(100000);
    std::thread c1(client, l, 1);
    std::thread c2(client, l, 2);    


    c1.join();
    c2.join();
    ser.join();

}
