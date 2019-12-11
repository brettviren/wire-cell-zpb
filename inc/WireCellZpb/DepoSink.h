#ifndef WIRECELL_ZPB_DEPOSINK_H
#define WIRECELL_ZPB_DEPOSINK_H

#include "WireCellIface/IDepoSink.h"
#include "WireCellZpb/NodeConfigurable.h"

namespace WireCell {
    namespace Zpb { 

        class DepoSink : public WireCell::IDepoSink,  public NodeConfigurable {
        public:
            DepoSink();
            virtual ~DepoSink() ;

            virtual bool operator()(const IDepo::pointer& depo);

            virtual WireCell::Configuration default_configuration() const;

        private:


            virtual bool validate() override;

            zio::portptr_t m_port;
        };
    }
}

#endif
