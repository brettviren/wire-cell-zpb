#include "WireCellZpb/FrameSource.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/NamedFactory.h"

WIRECELL_FACTORY(ZpbFrameSource, WireCell::Zpb::FrameSource,
                 WireCell::IFrameSource, WireCell::IConfigurable)

using namespace WireCell;

Zpb::FrameSource::FrameSource()
    : FlowConverter("inject")
{
}


bool Zpb::FrameSource::operator()(IFrame::pointer& frame)
{
    return pb2wct(frame);
}

