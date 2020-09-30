//===========================================================================
// Trustwave ltd. @{SRCH}
//								cdcm_worker_process.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 14 Jul 2019
// Comments:

//===========================================================================
//                              Include Files.
//===========================================================================

#include <thread>
#include "singleton_runner/authenticated_scan_server.hpp"
#include "zmq/zmq_helpers.hpp"
#include "message_worker.hpp"
#include "utils/action_manager.hpp"
std::unique_ptr<trustwave::ILogger> logger_ptr_u;
template<> int trustwave::authenticated_scan_server::run_as<::trustwave::process_type::worker>(size_t id)
{
    std::cout << "authenticated_scan_server running as worker id " << id << std::endl;
    LoggerSource::instance()->set_source(::trustwave::logger::worker, id);
    if(!Initialize(logger_ptr_u, conf_root)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    logger_ptr_ = logger_ptr_u.get();
    AU_LOG_INFO("%s",conf_->to_string().c_str());
    AU_LOG_INFO("Looking for plugins in  %s", conf_->plugins_dir_.c_str());
    auto sl_vec = action_manager::load(conf_->plugins_dir_, public_dispatcher_);
    std::thread worker_thread(message_worker::worker_loop,std::ref(ios_));
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> a= boost::asio::make_work_guard(ios_);
    ios_.run();
    worker_thread.join();
    return 0;
}
int main(int, char** argv)
{
    trustwave::zmq_helpers::version_assert(4, 0);
    trustwave::zmq_helpers::catch_signals();
    int rc =  trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::worker>(
        std::stoull(argv[1]));
    logger_ptr_u.reset();
    return rc;
}
