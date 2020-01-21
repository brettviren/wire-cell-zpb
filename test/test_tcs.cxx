#include "WireCellUtil/Logging.h"

#include "zio/message.hpp"
#include <unistd.h>
using namespace WireCell;

//const char* addr = "inproc://test_depozpb";
const char* addr = "tcp://127.0.0.1:5678";

// a very very ugly server
static
void server(Log::logptr_t l)
{
    zio::context_t ctx;
    zio::socket_t s(ctx, ZMQ_SERVER);
    s.bind(addr);

    std::map<uint32_t,uint32_t> rids;
    std::map<uint32_t, std::vector<int> > tosend;

    while (true) {
        zio::message_t msg;
        l->info("server: recv");
        auto res = s.recv(msg);
        assert(res);
        uint32_t rid = msg.routing_id();
        assert (rid > 0);

        // the "message"
        int them = *static_cast<int*>(msg.data());

        l->info("server: recvd {} {}", rid, them);

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
                zio::message_t msg(&them, sizeof(int));        
                msg.set_routing_id(orid);
                auto ses = s.send(msg, zio::send_flags::none);
                assert(ses);
            }
        }
        tosend.clear();
    }
}

static
void client(Log::logptr_t l, int me)
{
    zio::context_t ctx;
    zio::socket_t c(ctx, ZMQ_CLIENT);
    c.connect(addr);
    usleep(100000);
    while (true) {
        zio::message_t msg(&me, sizeof(int));        
        l->info("client {} send", me);
        c.send(msg, zio::send_flags::none);
        auto res = c.recv(msg);
        assert(res);
        int them = *static_cast<int*>(msg.data());
        l->info("client {} got {}", me, them);
    }
}
int main()
{
    Log::add_stderr(true, "debug");
    Log::set_level("debug", "zpb");
    Log::set_level("debug", "tst");
    Log::logptr_t l(Log::logger("tst"));
    l->info("test_tcs starting");

    std::thread ser(server, l);
    usleep(100000);
    std::thread c1(client, l, 1);
    std::thread c2(client, l, 2);    


    c1.join();
    c2.join();
    ser.join();

}
