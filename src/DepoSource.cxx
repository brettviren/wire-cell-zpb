#include "WireCellZpb/DepoSource.h"
#include "WireCellUtil/NamedFactory.h"

#include "wctzpb.pb.h"

WIRECELL_FACTORY(ZpbDepoSource, WireCell::Zpb::DepoSource,
                 WireCell::IDepoSource, WireCell::IConfigurable)

using namespace WireCell;

const std::string PORTNAME = "input";

Zpb::DepoSource::DepoSource()
    : NodeConfigurable(NodeConfigurable::node_config_t{
            "ZpbDepoSource",
            "wct-zpb-depo-source",
            {{PORTNAME,ZMQ_SUB}}})
{
}

Zpb::DepoSource::~DepoSource()
{
}

bool Zpb::DepoSource::validate()
{
    m_port = m_node.port(PORTNAME);
    return m_port != nullptr;
}

// this will probably move to its own file
namespace WireCell {
namespace Zpb {
    class Depo : public IDepo {
        wirecell::zpb::data::Depo m_depo;
        IDepo::pointer m_prior;
        Point m_pos;
    public:
        Depo(const wirecell::zpb::data::Depo& depo, IDepo::pointer prior = nullptr)
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
bool Zpb::DepoSource::operator()(IDepo::pointer& depo)
{
    depo = nullptr;

    wirecell::zpb::data::Depo out;
    bool is_eos = false;
    bool ok = recv(m_port, out, is_eos);
    if (!ok) { return false; }
    if (is_eos) { return true; }

    depo = std::make_shared<Zpb::Depo>(out);

    return true;
}

