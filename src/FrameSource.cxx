#include "WireCellZpb/FrameSource.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbFrameSource, WireCell::Zpb::FrameSource,
                 WireCell::IFrameSource, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "input";

Zpb::FrameSource::FrameSource()
    : FlowConverter("ZpbFrameSource", "wct-zpb-frame-source",
                    "inject", "inject-frames")
{
}


bool Zpb::FrameSource::operator()(IFrame::pointer& frame)
{
    return pb2wct(frame);
}

