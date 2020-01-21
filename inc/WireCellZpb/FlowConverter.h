/*! A generic mixin implements a single port node which participates in ZIO flow.
 *
 */

#ifndef WIRECELLZPB_FLOWCONVERTER
#define WIRECELLZPB_FLOWCONVERTER

#include "WireCellZpb/NodeConfigurable.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/Exceptions.h"
#include "wctzpb.pb.h"

namespace WireCell {
    namespace Zpb {

        template<class WCTDATA, class PBDATA>
        class FlowConverter : public NodeConfigurable {
            std::string m_direction, m_portname;
            bool m_had_eos{false};
            flowptr_t m_flow;
        public:
            
            typedef PBDATA pb_type;
            typedef WCTDATA wct_type;
            typedef std::shared_ptr<const wct_type> pointer;

            FlowConverter(std::string wct_type,
                          std::string nick,
                          std::string direction = "extract",
                          std::string portname = "flow")
                : NodeConfigurable({wct_type,nick,{{portname,ZMQ_CLIENT}}})
                , m_direction(direction)
                , m_portname(portname)
                { }


            // Use me as a WCT sink
            virtual bool wct2zpb(const pointer& wctdat) {
                zio::Message msg("FLOW");

                if (!wctdat) {
                    if (m_had_eos) {
                        m_flow->send_eot(msg);
                        m_flow->recv_eot(msg, m_timeout);
                        return false;
                    }
                    m_had_eos = true;
                    bool ok = m_flow->put(msg);
                    if (!ok) {
                        zio::Message eot;
                        m_flow->send_eot(eot);
                        return false;
                    }
                    return true;
                }

                PBDATA out = Zpb::convert(wctdat);
                zio::message_t pl = pack(out);
                msg.add(std::move(pl));

                bool ok = m_flow->put(msg);
                if (!ok) {                  // got eot from remote
                    zio::Message eot;
                    m_flow->send_eot(eot);
                    return false;
                }
                return true;
            }

            // Use me as a WCT source
            virtual bool pb2wct(pointer& wctdat) {
                wctdat = nullptr;

                zio::Message msg;
                bool ok = m_flow->get(msg, m_timeout);
                if (!ok) {
                    zio::Message eot;
                    m_flow->send_eot(eot);
                    return false;           // timeout, eot or other error
                }

                const zio::multipart_t& pls = msg.payload();
                if (!pls.size()) {           // EOS
                    return true;
                }

                PBDATA out;
                unpack(pls[0], out);
                wctdat = Zpb::convert(out);
                return true;
            }

        private:

            virtual void user_default_configuration(Configuration& cfg) const {
                cfg["credits"] = 10;
            }
            virtual void user_configure(const Configuration& cfg) {
                int credits = cfg["credits"].asInt();
                m_flow = make_flow(m_portname, m_direction, credits);
                if (!m_flow) {
                    THROW(RuntimeError() << errmsg{"failed to make flow"});
                }
                if (m_direction == "ingest") {
                    m_flow->flush_pay();
                }
                else {
                    m_flow->slurp_pay(0);
                }
            }


        };

    }
}

#endif

