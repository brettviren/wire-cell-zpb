/**
   NodeConfigurable : a mix-in class for all ZPB components which have
   a ZIO node.

 */

#ifndef WIRECELL_ZPB_NODECONFIGURABLE
#define WIRECELL_ZPB_NODECONFIGURABLE

#include "WireCellIface/IConfigurable.h"
#include "WireCellUtil/Logging.h"
#include "zio/node.hpp"
#include "zio/types.hpp"

#include "wctzpb.pb.h"

#include <unordered_map>


namespace WireCell {
    namespace Zpb {
        
        class NodeConfigurable : public WireCell::IConfigurable {
        public:
            NodeConfigurable(const std::string& wct_type, // ie, as in WIRECELL_FACTORY
                             const std::string& default_name = "",
                             zio::level::MessageLevel default_level = zio::level::info);
            virtual ~NodeConfigurable();

            /// IConfigurable
            virtual void configure(const WireCell::Configuration& config);
            virtual WireCell::Configuration default_configuration() const;
            
            // message sending/receiving
            bool send_eos(zio::portptr_t port);
            bool send(zio::portptr_t port, ::google::protobuf::Message& msg);
            bool recv(zio::portptr_t port, ::google::protobuf::Message& msg);
            bool recv(zio::portptr_t port, ::google::protobuf::Message& msg, bool& is_eos);

        protected:

            // subclass may override this (and possibly still call it)
            virtual void online();

            zio::Node m_node;
            std::string m_wct_type;
            std::unordered_map<std::string, zio::portptr_t> m_ports;
            zio::headerset_t m_headers;
            zio::level::MessageLevel m_level;
            Log::logptr_t l;                
        };
    }
}

#endif
