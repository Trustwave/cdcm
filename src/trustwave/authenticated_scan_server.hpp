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
#include <thread>
#include <zmq.hpp>

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
struct config
{
    size_t session_idle_time = 10;

};
struct authenticated_scan_server
{
    std::unique_ptr<::trustwave::ILogger> logger_ptr;
    Dispatcher<Action_Base> prv_dispatcher;
    Dispatcher<Action_Base> public_dispatcher;
    sessions_container sessions;
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
    void run()
    {
        zmq::context_t ctx(1);
        std::thread t3(message_broker::th_func, std::ref(ctx));
        std::vector<std::thread> tp;
        for (unsigned int i = 0; i < /*std::thread::hardware_concurrency() - 2*/1; ++i)        //context+broker
                        {
            tp.push_back(std::move(std::thread(message_worker::th_func, std::ref(ctx), "inproc://broker")));

        }

        t3.join();
        if (zmq_helpers::interrupted)
            printf("W: interrupt received, shutting down...\n");

    }
private:
    authenticated_scan_server() :
                    logger_ptr(nullptr)
    {
#undef uint_t
        //
        //  configuration path.
        //  -------------------
        const std::string root_conf("/home/ascohen/dev/samba_fresh/samba/trustwave");
        if (!Initialize(logger_ptr, root_conf, ::trustwave::logger::agent)) {
            std::cerr << "failed to initialize the logger!!!" << std::endl;
            abort();
            //return -1;
        }
        ::trustwave::LoggerSource::instance()->set_source(::trustwave::logger::agent);
        zmq_helpers::version_assert(4, 0);
        zmq_helpers::catch_signals();

    }

};

}
#include "log_macros.hpp"

#endif /* TRUSTWAVE_AUTHENTICATED_SCAN_SERVER_HPP_ */
