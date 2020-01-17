#ifndef WIRECELL_ZPB_DEPOSOURCE
#define WIRECELL_ZPB_DEPOSOURCE

#include "WireCellIface/IDepoSource.h"
#include "NodeConfigurable.h"

namespace WireCell {
    namespace Zpb {

        class DepoSource : public WireCell::IDepoSource, public NodeConfigurable {
        public:
            DepoSource();
            virtual ~DepoSource();

            virtual bool operator()(IDepo::pointer& depo);

        private:

            virtual void online() override;

            flowptr_t m_flow;


        };
    }

}

#endif
