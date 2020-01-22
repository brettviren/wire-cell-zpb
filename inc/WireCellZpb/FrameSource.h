#ifndef WIRECELL_ZPB_FRAMESOURCE
#define WIRECELL_ZPB_FRAMESOURCE

#include "WireCellIface/IFrameSource.h"
#include "FlowConverter.h"

namespace WireCell {
    namespace Zpb {

        class FrameSource : public WireCell::IFrameSource,
                           public FlowConverter<WireCell::IFrame, wctzpb::Frame> {
        public:
            FrameSource();

            virtual bool operator()(IFrame::pointer& frame);

        };
    }

}

#endif
