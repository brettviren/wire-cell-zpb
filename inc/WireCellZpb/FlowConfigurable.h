/*! 
 * FlowConfigurable provides common configuration for any component
 * that is a sink/extractor or source/injector of WCT / ZIO flow data.
 *
 * It provides a ZIO flow node with a single port.
 *
 * It provides configurable management of the sending of the BOT to
 * assist in ZPB.
 *
 */

#ifndef WIRECELLZPB_FLOWCONFIGURABLE
#define WIRECELLZPB_FLOWCONFIGURABLE

#include "WireCellIface/IConfigurable.h"
#include "WireCellIface/ITerminal.h"
#include "WireCellUtil/Logging.h"
#include "zio/node.hpp"
#include "zio/flow.hpp"

#include "wctzpb.pb.h"


namespace WireCell {
    namespace Zpb {

        class FlowConfigurable : public WireCell::IConfigurable,
                                 public WireCell::ITerminal {
        public:

            FlowConfigurable(const std::string& direction,
                             const std::string& nodename = "");
            virtual ~FlowConfigurable();

            virtual WireCell::Configuration default_configuration() const;
            virtual void configure(const WireCell::Configuration& config);
            
            virtual void finalize();

        protected:
            zio::Node m_node;
            int m_timeout{1000}, m_credit{10};
            std::string m_portname{"flow"}, m_direction{"extract"}, m_bot_label{""};
            zio::level::MessageLevel m_level{zio::level::info};
            int m_stype{ZMQ_CLIENT};
            zio::headerset_t m_headers{};

            Log::logptr_t l;

            /// Subclass should set this to the type of protobuf payload message
            std::string m_type_name{""};

            /// Subclass may use this for all but BOT.
            typedef std::unique_ptr<zio::flow::Flow> flowptr_t;
            flowptr_t m_flow;

            /// Subclass must call this before any actual flow.  It is
            /// safe to call at the top of each execution.
            bool pre_flow();

            /// Give subclass a chance to add to a configuration
            virtual void user_default_configuration(WireCell::Configuration& cfg) const {};

            /// Give subclass a chance to read a configuration.  Ports
            /// are not yet online.
            virtual bool user_configure(const WireCell::Configuration& cfg)
                {return true;};

            /// Called after going online and before configuration phase is over
            virtual bool user_online() { return true; }

            /// Pack the PB message into a ZIO payload by wrapping it
            /// in an "any" held by WCT's proto "Payload" object
            zio::message_t pack(::google::protobuf::Message& pbmsg);

            /// Unpack ZIO payload to a PB message of expected type.
            void unpack(const zio::message_t& spmsg, ::google::protobuf::Message& pbmsg);

        private:
            // assure pre_flow() body called just once.
            bool m_did_bot{false};
       };
    }
}


#endif


