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
#include "zio/types.hpp"

#include "google/protobuf/message.h"

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
            
            bool selftest();

        protected:

            // ZIO message sending/receiving
            bool send_eos(zio::portptr_t port, ::google::protobuf::Message& msg);
            bool send(zio::portptr_t port, ::google::protobuf::Message& msg);
            bool recv(zio::portptr_t port, ::google::protobuf::Message& msg);
            bool recv(zio::portptr_t port, ::google::protobuf::Message& msg, bool& is_eos);


            /// Subclass may override in order to perform any
            /// validation or other operation just before we go
            /// online.
            virtual bool validate() { return true; }
            void online();


            zio::Node m_node;
            node_config_t m_nc;
            Log::logptr_t l;                
        };
    }
}

#endif
