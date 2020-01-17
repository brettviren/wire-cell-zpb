/**

   NodeConfigurable : a mix-in class for all ZPB components which have
   a ZIO node.  This takes care of the brunt of configuration.  The
   "real" component class should inherit from this as well as a graph
   component interface.

 */

#ifndef WIRECELL_ZPB_NODECONFIGURABLE
#define WIRECELL_ZPB_NODECONFIGURABLE

#include "WireCellIface/IConfigurable.h"
#include "WireCellUtil/Logging.h"
#include "zio/node.hpp"
#include "zio/flow.hpp"

#include "google/protobuf/message.h"

#include <memory>
#include <unordered_map>


namespace WireCell {
    namespace Zpb {
        
        class NodeConfigurable : public WireCell::IConfigurable {
        public:

            /// Sub class must provide configuration:
            struct node_config_t {
                // The WCT component type name (ie, what is given to
                // WIRECELL_FACTORY macro).
                std::string wct_type; 
                // A default node nick name, user config may override
                std::string nick;
                // Must provide a map from allowed port names to a
                // default ZMQ socket type number, which also marks it
                // as being "input" or "output".
                std::map<std::string, int> ports; 
                // Leave as zero to generate "random" origin number
                zio::origin_t origin{0}; 
                // Extra node headers may be provided.  WCT-Type is
                // constructed by this class.
                zio::headerset_t headers{};
                // Default level for output ports.
                zio::level::MessageLevel level{zio::level::info};
            };

            NodeConfigurable(const node_config_t& mycfg);
            virtual ~NodeConfigurable();

            /// IConfigurable
            virtual WireCell::Configuration default_configuration() const;
            virtual void configure(const WireCell::Configuration& config);

        protected:

            /// Subclass may override in order to perform any
            /// validation or other operation just before we go
            /// online.
            virtual bool validate() { return true; }

            /// Pack the PB message into a ZIO payload by wrapping it
            /// in an "any" held by WCT's proto "Payload" object
            zio::message_t pack(::google::protobuf::Message& pbmsg);


            /// Unpack ZIO payload to a PB message of expected type.
            void unpack(const zio::message_t& spmsg, ::google::protobuf::Message& pbmsg);

            /// Return a Flow on a port.
            typedef std::unique_ptr<zio::flow::Flow> flowptr_t;
            flowptr_t make_flow(const std::string& portname,
                                const std::string& direction,
                                int credits);


            /// Subclass gets notification of being online.
            /// After this, ports should be hooked up.
            virtual void online();

            zio::Node m_node;
            node_config_t m_nc;
            Log::logptr_t l;                
            int m_timeout;      // a timeout to use when recv'ing

        private:
            void go_online();
        };
    }
}

#endif
