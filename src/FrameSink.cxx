#include "WireCellZpb/FrameSink.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbFrameSink, WireCell::Zpb::FrameSink,
                 WireCell::IFrameSink, WireCell::IConfigurable)

using namespace WireCell;

Zpb::FrameSink::FrameSink()
    : FlowConverter("extract")
{
}

bool Zpb::FrameSink::operator()(const WireCell::IFrame::pointer& wctdat)
{
    return wct2zpb(wctdat);
}
