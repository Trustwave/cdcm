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

#ifndef TRUSTWAVE_AUTHENTICATED_SCAN_SERVER_HPP_
#define TRUSTWAVE_AUTHENTICATED_SCAN_SERVER_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "misc/sessions_container.hpp"
#include "misc/dispatcher.hpp"
#include "misc/action.hpp"
#include "misc/Logger/include/Logger.h"
#include "message_broker.hpp"
#include "message_worker.hpp"
#include "zmq_message.hpp"
#include "zmq_helpers.hpp"
#include "settings.hpp"
#include <thread>
#include <zmq.hpp>

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

struct authenticated_scan_server
{
    std::unique_ptr<::trustwave::ILogger> logger_ptr;
    Dispatcher<Action_Base> prv_dispatcher;
    Dispatcher<Action_Base> public_dispatcher;
    sessions_container sessions;
    cdcm_settings settings;
    authenticated_scan_server(const authenticated_scan_server&) = delete;
    authenticated_scan_server& operator=(const authenticated_scan_server &) = delete;
    authenticated_scan_server(authenticated_scan_server &&) = delete;
    authenticated_scan_server & operator=(authenticated_scan_server &&) = delete;

    static auto& instance()
    {
        static authenticated_scan_server app;
        return app;
    }
    ILogger* logger()
    {
        return logger_ptr.get();
    }
    void run();
private:
#undef uint_t
    authenticated_scan_server();

};

}
#include "log_macros.hpp"

#endif /* TRUSTWAVE_AUTHENTICATED_SCAN_SERVER_HPP_ */
