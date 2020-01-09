/*!
  @brief Conversionn between Wire-Cell IData interfaces and protobuf objects
*/
#ifndef WIRECELL_ZPB_CONVERTERS
#define WIRECELL_ZPB_CONVERTERS

#include "WireCellIface/IDepo.h"
#include "wctzpb.pb.h"

namespace WireCell {
    namespace Zpb {

        /// Depo PB to WCT
        IDepo::pointer convert(const wctzpb::Depo& depo);
                     
        /// Depo WCT to PB
        wctzpb::Depo convert(const IDepo::pointer& depo);
                     

    }
}

#endif
