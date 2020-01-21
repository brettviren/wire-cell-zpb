#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/DepoSink.h"
#include "WireCellIface/SimpleDepo.h"
#include "WireCellUtil/Logging.h"
#include <iostream>

using namespace WireCell;

using namespace std;

void depo_extraction(Log::logptr_t l, Zpb::DepoSink& us)
{
    l->info("sending depo1");

    IDepo::pointer depo1 = std::make_shared<SimpleDepo>(6.9,WireCell::Point(0,0,0));
    bool ok = us(depo1);
    assert(ok);

    l->info("sending eos");
    ok = us(nullptr);
    assert(ok);
}

void depo_injection(Log::logptr_t l, Zpb::DepoSource& ds)
{

    l->info("recving depo2");

    IDepo::pointer depo2 = nullptr;
    bool ok = ds(depo2);
    assert(ok);
    assert(depo2);

    IDepo::pointer depo3 = nullptr;
    l->info("recving eos");
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

    Zpb::DepoSink us;           // up stream sinks IDepo produces messages
    Zpb::DepoSource ds;         // down stream sources IDepo consumes messages

    auto us_cfg = us.default_configuration();
    auto ds_cfg = ds.default_configuration();

    us_cfg["ports"]["extract-depos"]["stype"] = ZMQ_PAIR;
    us_cfg["ports"]["extract-depos"]["binds"][0] = "inproc://test_depozpb";
    ds_cfg["ports"]["inject-depos"]["stype"] = ZMQ_PAIR;
    ds_cfg["ports"]["inject-depos"]["connects"][0]["nodename"] = us_cfg["nick"];
    ds_cfg["ports"]["inject-depos"]["connects"][0]["portname"] = "extract-depos";

    ds_cfg["verbose"] = 1;
    us_cfg["verbose"] = 1;

    cerr << "DepoSink config:\n" << us_cfg << "\n";
    cerr << "DepoSource config:\n" << ds_cfg << "\n";

    l->info("configuring upstream");
    us.configure(us_cfg);       // binds
    l->info("configuring downstream");
    ds.configure(ds_cfg);       // configures, waits for us to be online

    l->info("starting threads");
    std::thread tus(depo_extraction, l, std::ref(us));
    std::thread tds(depo_injection, l, std::ref(ds));

    l->info("joining threads");
    tus.join();
    tds.join();

    l->info("bye");
    return 0;
}
