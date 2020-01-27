#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

Zpb::DepoSource::DepoSource()
    : FlowConverter("inject")
{
}


bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    return pb2wct(depo);
}

