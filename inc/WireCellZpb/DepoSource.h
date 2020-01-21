#ifndef WIRECELL_ZPB_DEPOSOURCE
#define WIRECELL_ZPB_DEPOSOURCE

#include "WireCellIface/IDepoSource.h"
#include "FlowConverter.h"

namespace WireCell {
    namespace Zpb {

        class DepoSource : public WireCell::IDepoSource,
                           public FlowConverter<WireCell::IDepo, wctzpb::Depo> {
        public:
            DepoSource();

            virtual bool operator()(IDepo::pointer& depo);

        };
    }

}

#endif
