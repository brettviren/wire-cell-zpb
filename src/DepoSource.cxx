#include "WireCellZpb/DepoSource.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "input";

Zpb::DepoSource::DepoSource()
    : FlowConverter("ZpbDepoSource", "wct-zpb-depo-source",
                    "inject", "inject-depos")
{
}


bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    return pb2wct(depo);
}

