//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														authenticated_scan_server.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 19 May 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_SINGLETON_RUNNER_AUTHENTICATED_SCAN_SERVER_HPP_
#define TRUSTWAVE_COMMON_SINGLETON_RUNNER_AUTHENTICATED_SCAN_SERVER_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "settings.hpp"
#include "action.hpp"
#include "configurable.hpp"
#include "configuration.hpp"
#include "typedefs.hpp"
#include "dispatcher.hpp"
#include "Logger/include/Logger.h"
#include "sessions_cache/shared_mem_sessions_cache.hpp"

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================

namespace trustwave {

struct authenticated_scan_server:public configurable<cdcm_settings> {
    std::unique_ptr <ILogger> logger_ptr;
    Dispatcher <Action_Base> public_dispatcher;
    Dispatcher <configuration> service_conf_reppsitory;
    boost::shared_ptr <shared_mem_sessions_cache> sessions;
    authenticated_scan_server(const authenticated_scan_server&) = delete;
    authenticated_scan_server& operator=(const authenticated_scan_server &) = delete;
    authenticated_scan_server(authenticated_scan_server &&) = delete;
    authenticated_scan_server & operator=(authenticated_scan_server &&) = delete;
    std::shared_ptr<cdcm_settings> settings()
    {
        return conf_;
    }
    static auto& instance()
    {
        static authenticated_scan_server app;
        return app;
    }
    ILogger* logger()
    {
        return logger_ptr.get();
    }
    template<typename T>
    int run_as(size_t instance_id = 0);
    boost::shared_ptr <session> get_session(const std::string& session_id);
private:
#undef uint_t
    authenticated_scan_server();

};

}
#include "log_macros.hpp"

#endif /* TRUSTWAVE_COMMON_SINGLETON_RUNNER_AUTHENTICATED_SCAN_SERVER_HPP_ */
