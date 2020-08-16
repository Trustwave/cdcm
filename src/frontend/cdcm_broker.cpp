/*
 * reg_cli_test.cpp
 *
 *  Created on: Apr 17, 2019
 *      Author: root
 */
//===========================================================================
//                              Include Files.
//===========================================================================
#include <zmq.hpp>
#include <boost/asio.hpp>
#include "singleton_runner/authenticated_scan_server.hpp"
#include "zmq/zmq_helpers.hpp"
#include "message_broker.hpp"
#include "maintenance.hpp"
std::unique_ptr<trustwave::ILogger> logger_ptr_u;
template<> int trustwave::authenticated_scan_server::run_as<::trustwave::process_type::broker>(size_t)
{
    zmq::context_t ctx(1);
    boost::asio::io_service ios;
    LoggerSource::instance()->set_source(::trustwave::logger::broker);
    if(!Initialize(logger_ptr_u, conf_root)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    logger_ptr_ = logger_ptr_u.get();
    AU_LOG_INFO("%s",conf_->to_string().c_str());
    std::thread broker_thread(message_broker::th_func, std::ref(ctx), std::ref(ios));
    maintenance m(ios);
    ios.run();
    broker_thread.join();
    return 0;
}
int main(int, const char**)
{
    trustwave::zmq_helpers::version_assert(4, 0);
    trustwave::zmq_helpers::catch_signals();

    int rc =  trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::broker>();
    logger_ptr_u.reset();
    return rc;
}
