#include "WireCellZpb/DepoSink.h"
#include "WireCellUtil/NamedFactory.h"


WIRECELL_FACTORY(ZpbDepoSink, WireCell::Zpb::DepoSink,
                 WireCell::IDepoSink, WireCell::IConfigurable)

using namespace WireCell;

Zpb::DepoSink::DepoSink()
    : FlowConverter<IDepo, wctzpb::Depo>("extract")
{
}

bool Zpb::DepoSink::operator()(const WireCell::IDepo::pointer& wctdat)
{
    return wct2zpb(wctdat);
}

