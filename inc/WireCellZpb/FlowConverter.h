/*! A generic mixin implements a single port node which participates in ZIO flow.
 *
 */

#ifndef WIRECELLZPB_FLOWCONVERTER
#define WIRECELLZPB_FLOWCONVERTER

#include "WireCellZpb/FlowConfigurable.h"
#include "WireCellZpb/Converters.h"
#include "WireCellUtil/Exceptions.h"

namespace WireCell {
    namespace Zpb {

        template<class WCTDATA, class PBDATA>
        class FlowConverter : public FlowConfigurable {
            bool m_had_eos{false};
        public:
            
            typedef PBDATA pb_type;
            typedef WCTDATA wct_type;
            typedef std::shared_ptr<const wct_type> pointer;

            FlowConverter(std::string direction,
                          std::string nick = "",
                          std::string portname = "flow")
                : FlowConfigurable(direction, nick) {
                m_portname = portname;
                pb_type pbobj;
                m_type_name = pbobj.GetTypeName();
            }

            // Use me as a WCT sink / ZIO flow extraction
            virtual bool wct2zpb(const pointer& wctdat) {
                pre_flow();
                if (!m_flow) {
                    return false;
                }

                zio::Message msg("FLOW");

                if (!wctdat) {  // eos
                    if (m_had_eos) {
                        finalize();
                        return false;
                    }
                    m_had_eos = true;
                    bool ok = m_flow->put(msg);
                    if (!ok) {
                        zio::Message eot;
                        m_flow->send_eot(eot);
                        m_flow = nullptr;
                        return false;
                    }
                    return true;
                }
                // non EOT message resets
                m_had_eos = false;

                PBDATA out = Zpb::convert(wctdat);
                zio::message_t pl = pack(out);
                msg.add(std::move(pl));

                bool ok = m_flow->put(msg);
                if (!ok) {                  // got eot from remote
                    zio::Message eot;
                    m_flow->send_eot(eot);
                    m_flow = nullptr;
                    return false;
                }
                return true;
            }

            // Use me as a WCT source / ZIO flow injection
            virtual bool pb2wct(pointer& wctdat) {
                pre_flow();
                if (!m_flow) {
                    return false;
                }

                wctdat = nullptr;

                zio::Message msg;
                bool ok = m_flow->get(msg, m_timeout);
                if (!ok) {
                    zio::Message eot;
                    m_flow->send_eot(eot);
                    m_flow = nullptr;
                    return false;           // timeout, eot or other error
                }

                const zio::multipart_t& pls = msg.payload();
                if (!pls.size()) {           // EOS
                    if (m_had_eos) {         // 2nd
                        finalize();
                        return false;
                    }
                    m_had_eos = true;
                    return true;
                }
                m_had_eos = false;

                PBDATA out;
                unpack(pls[0], out);
                wctdat = Zpb::convert(out);
                return true;
            }

        private:


        };
    }
}

#endif

