#ifndef WIRECELL_ZPB_DEPOSINK_H
#define WIRECELL_ZPB_DEPOSINK_H

#include "WireCellIface/IDepoSink.h"
#include "WireCellIface/IConfigurable.h"

#include "zio/node.hpp"

#include "zio/format.hpp"


namespace WireCell {
    namespace Zpb { 

        class DepoSink : public IDepoSink, public IConfigurable
        {
        public:
            DepoSink();
            virtual ~DepoSink() ;

            // IDepoSink
            virtual bool operator()(const IDepo::pointer& depo);

            /// IConfigurable
            virtual void configure(const WireCell::Configuration& config);
            virtual WireCell::Configuration default_configuration() const;

        private:
            zio::Node m_node;
            zio::portptr_t m_port;
            zio::level::MessageLevel m_level;
        };
    }
}

#endif
