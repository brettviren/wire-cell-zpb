/**
   Define various implementations of WireCell::IData which wrap around PB objects.
 */

#ifndef WIRECELL_ZPB_DATAIMPS
#define WIRECELL_ZPB_DATAIMPS

#include "wctzpb.pb.h"

namespace WireCell {
    namespace Zpb {

        class Depo : public IDepo {
            wctzpb::Depo m_depo;
            IDepo::pointer m_prior;
            Point m_pos;
        public:
            Depo(const wctzpb::Depo& depo, IDepo::pointer prior = nullptr)
                : m_depo(depo), m_prior(prior)
                , m_pos(depo.pos().x(), depo.pos().y(), depo.pos().z())
                {    }
          
            const Point& pos() const { return m_pos; }

            double time() const { return m_depo.time(); }

            double charge() const { return m_depo.charge(); }

            double energy() const { return m_depo.energy(); }

            int id() const { return m_depo.trackid(); }

            int pdg() const { return m_depo.pdg(); }
	
            pointer prior() const { return m_prior; }

            double extent_long() const { return m_depo.extent_long(); }

            double extent_tran() const { return m_depo.extent_tran(); }

        };
    }
}

#endif
