#ifndef WIRECELL_ZPB_DEPOSINK_H
#define WIRECELL_ZPB_DEPOSINK_H

#include "WireCellIface/IDepoSink.h"
#include "FlowConverter.h"

namespace WireCell {
    namespace Zpb { 

        class DepoSink : public FlowConverter<WireCell::IDepo, wctzpb::Depo>,
                         public WireCell::IDepoSink
        {
        public:
            DepoSink();

            virtual bool operator()(const WireCell::IDepo::pointer& wctdat);

        };
    }
}

#endif
