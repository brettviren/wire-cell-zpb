#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/DepoSink.h"
#include "WireCellIface/SimpleDepo.h"
#include "WireCellUtil/Logging.h"
#include <iostream>

using namespace WireCell;

using namespace std;

int main()
{
    Log::add_stderr(true, "debug");
    Log::set_level("debug", "zpb");
    Log::logptr_t l(Log::logger("tst"));
    l->info("test_depozpb starting");

    Zpb::DepoSink us;           // up stream sinks IDepo produces messages
    Zpb::DepoSource ds;         // down stream sources IDepo consumes messages

    auto us_cfg = us.default_configuration();
    auto ds_cfg = ds.default_configuration();

    us_cfg["ports"]["output"]["stype"] = ZMQ_PUSH;
    ds_cfg["ports"]["input"]["stype"] = ZMQ_PULL;
    ds_cfg["ports"]["input"]["connects"][0]["nodename"] = us_cfg["nick"];
    ds_cfg["ports"]["input"]["connects"][0]["portname"] = "output";

    ds_cfg["verbose"] = 1;
    us_cfg["verbose"] = 1;

    cerr << "DepoSink config:\n" << us_cfg << "\n";
    cerr << "DepoSource config:\n" << ds_cfg << "\n";

    us.configure(us_cfg);       // binds
    ds.configure(ds_cfg);       // configures, waits for us

    l->debug("sleeping");
    sleep(1);

    l->debug("sending depo1");

    IDepo::pointer depo1 = std::make_shared<SimpleDepo>(6.9,WireCell::Point(0,0,0));
    bool ok = us(depo1);
    assert(ok);

    l->debug("recving depo2");

    IDepo::pointer depo2 = nullptr;
    ok = ds(depo2);
    assert(ok);
    assert(depo2);

    assert(depo1 != depo2);
    assert(depo1->time() == depo2->time());

    l->debug("sending eos");
    ok = us(nullptr);
    assert(ok);
    IDepo::pointer depo3 = nullptr;
    l->debug("recving eos");
    ok = ds(depo3);
    assert(ok);
    assert(depo3 == nullptr);

    l->debug("bye");
    return 0;
}
