/**
   Define various implementations of WireCell::IData which wrap around PB objects.
 */

#ifndef WIRECELL_ZPB_DATAIMPS
#define WIRECELL_ZPB_DATAIMPS

#include "WireCellIface/SimpleDepo.h"
#include "WireCellIface/SimpleTrace.h"
#include "WireCellIface/SimpleFrame.h"

#include "wctzpb.pb.h"

namespace WireCell {
    namespace Zpb {

        class Depo : public SimpleDepo {
        public:
            Depo(const wctzpb::Depo& depo, IDepo::pointer prior = nullptr);
        };

        class Trace : public SimpleTrace {
        public:
            Trace(const wctzpb::Trace& trace);
        };
        
        class Frame : public SimpleFrame {
        public:
            Frame(const wctzpb::Frame& frame);
        };
    }
}

#endif
