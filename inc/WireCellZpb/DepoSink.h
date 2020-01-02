#ifndef WIRECELL_ZPB_DEPOSINK_H
#define WIRECELL_ZPB_DEPOSINK_H

#include "WireCellIface/IDepoSink.h"
#include "WireCellZpb/NodeConfigurable.h"

#include <memory>

namespace WireCell {
    namespace Zpb { 

        class DepoSink : public WireCell::IDepoSink,  public NodeConfigurable {
        public:
            DepoSink();
            virtual ~DepoSink() ;

            virtual bool operator()(const IDepo::pointer& depo);

        private:


            virtual void online() override;

            std::unique_ptr<zio::flow::Flow> m_flow;

            bool m_had_eos{false};
        };
    }
}

#endif
