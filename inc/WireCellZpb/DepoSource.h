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

            virtual void user_default_configuration(WireCell::Configuration& cfg) const;
            virtual void user_configure(const WireCell::Configuration& cfg);

            flowptr_t m_flow;


        };
    }

}

#endif
