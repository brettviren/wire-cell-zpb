#ifndef WIRECELL_ZPB_DEPOSINK_H
#define WIRECELL_ZPB_DEPOSINK_H

#include "WireCellIface/IDepoSink.h"
#include "NodeConfigurable.h"

#include <memory>

namespace WireCell {
    namespace Zpb { 

        class DepoSink : public WireCell::IDepoSink,  public NodeConfigurable {
        public:
            DepoSink();
            virtual ~DepoSink() ;

            virtual bool operator()(const IDepo::pointer& depo);

        private:


            virtual void user_default_configuration(WireCell::Configuration& cfg) const;
            virtual void user_configure(const WireCell::Configuration& cfg);

            bool m_had_eos{false};

            flowptr_t m_flow;
        };
    }
}

#endif
