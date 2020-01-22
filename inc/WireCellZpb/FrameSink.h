#ifndef WIRECELL_ZPB_FRAMESINK_H
#define WIRECELL_ZPB_FRAMESINK_H

#include "WireCellIface/IFrameSink.h"
#include "FlowConverter.h"

namespace WireCell {
    namespace Zpb { 

        class FrameSink : public FlowConverter<WireCell::IFrame, wctzpb::Frame>,
                          public WireCell::IFrameSink
        {
        public:
            FrameSink();

            virtual bool operator()(const WireCell::IFrame::pointer& frame);

        };
    }
}

#endif
