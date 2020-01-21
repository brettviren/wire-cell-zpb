/*!
  @brief Conversionn between Wire-Cell IData interfaces and protobuf objects
*/
#ifndef WIRECELL_ZPB_CONVERTERS
#define WIRECELL_ZPB_CONVERTERS

#include "WireCellIface/IDepo.h"
#include "WireCellIface/IFrame.h"
#include "wctzpb.pb.h"

namespace WireCell {
    namespace Zpb {

        /// Depo PB to WCT
        IDepo::pointer convert(const wctzpb::Depo& depo);
                     
        /// Depo WCT to PB
        wctzpb::Depo convert(const IDepo::pointer& depo);
                     
        /// Frame PB to WCT
        IFrame::pointer convert(const wctzpb::Frame& frame);
                     
        /// Frame WCT to PB
        wctzpb::Frame convert(const IFrame::pointer& frame);
                     

    }
}

#endif
