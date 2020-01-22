#include "WireCellZpb/FrameSink.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbFrameSink, WireCell::Zpb::FrameSink,
                 WireCell::IFrameSink, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "output";

Zpb::FrameSink::FrameSink()
    : FlowConverter("ZpbFrameSink", "wct-zpb-frame-sink",
                    "extract", "extract-frames")
{
}

bool Zpb::FrameSink::operator()(const WireCell::IFrame::pointer& wctdat)
{
    return wct2zpb(wctdat);
}
