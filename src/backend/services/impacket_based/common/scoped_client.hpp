//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														scoped_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/26/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_SCOPED_CLIENT_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_SCOPED_CLIENT_HPP
#include "client.hpp"
#include "impacket_service_configuration.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../../../utils/session_to_client_container.hpp"

namespace trustwave {
    struct scoped_client:public configurable<impacket_service_configuration> {
        scoped_client(std::shared_ptr<trustwave::cdcm_client> c, const std::string& s):
            c_(c), s_(s)
        {
            if(this->init_conf(authenticated_scan_server::instance().service_conf_repository)) {
                AU_LOG_INFO("%s", conf_->to_string().c_str());
            }
        }
        ~scoped_client() {
            if(c_ ) {
                if(c_->connected())
                {
                    auto s_to_c = authenticated_scan_server::instance().process_specific_repository().find_as<sessions_to_clients>();
                    s_to_c->set_client(c_,s_,conf_->idle_client_timeout);
                }
            }
        }
        std::shared_ptr<trustwave::cdcm_client> c_;
        std::string s_;
    };
}
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_SCOPED_CLIENT_HPP