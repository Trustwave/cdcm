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
#include "workers_monitor.hpp"
#include "maintenance.hpp"
template<> int trustwave::authenticated_scan_server::run_as<::trustwave::process_type::broker>(size_t)
{
    zmq::context_t ctx(1);
    boost::asio::io_service ios;
    namespace bp = boost::process;
    LoggerSource::instance()->set_source(::trustwave::logger::broker);
    if(!Initialize(logger_ptr_, conf_root)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    std::thread broker_thread(message_broker::th_func, std::ref(ctx), std::ref(ios));
    workers_monitor monitor(ios);
    maintenance m(ios);
    monitor.run();
    ios.run();
    broker_thread.join();

    return 0;
}
int main(int, const char**)
{
    trustwave::zmq_helpers::version_assert(4, 0);
    trustwave::zmq_helpers::catch_signals();

    return trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::broker>();
}
