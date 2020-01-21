#ifndef WIRECELL_ZPB_FRAMESINK_H
#define WIRECELL_ZPB_FRAMESINK_H

#include "WireCellIface/IFrameSink.h"
#include "NodeConfigurable.h"

#include <memory>

namespace WireCell {
    namespace Zpb { 

        class FrameSink : public WireCell::IFrameSink,  public NodeConfigurable {
        public:
            FrameSink();
            virtual ~FrameSink() ;

            virtual bool operator()(const IFrame::pointer& frame);

        private:


            virtual void user_default_configuration(WireCell::Configuration& cfg) const;
            virtual void user_configure(const WireCell::Configuration& cfg);

            bool m_had_eos{false};

            flowptr_t m_flow;
        };
    }
}

#endif
