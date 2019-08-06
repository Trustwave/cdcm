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
#include <boost/process.hpp>
#include <boost/asio.hpp>

#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/zmq_helpers.hpp"
#include "frontend/message_broker.hpp"
template<>
int trustwave::authenticated_scan_server::run_as <::trustwave::process_type::broker>(size_t)
{

    zmq::context_t ctx(1);
    boost::asio::io_service ios;
    namespace bp = boost::process;
    std::vector <char> buf(4096);
    const std::string root_conf("/home/ascohen/dev/samba_fresh/samba/trustwave");
    LoggerSource::instance()->set_source(::trustwave::logger::broker);
    if (!Initialize(logger_ptr, root_conf)) {
            std::cerr << "failed to initialize the logger!!!" << std::endl;
            abort();
        }
    std::thread broker_thread(message_broker::th_func, std::ref(ctx));

    std::vector <bp::child> v;
    for (size_t i = 1; i <= 1; ++i){

        v.emplace_back(bp::child("/home/ascohen/dev/samba_fresh/samba/bin/cdcm_worker", std::to_string(i), ios));

    }
    //todo assaf add worker threads as well
    ios.run();
    broker_thread.join();
    if (zmq_helpers::interrupted){
        printf("W: interrupt received, shutting down...\n");
        for (auto&& p : v){
            ::kill(p.native_handle(), SIGTERM);
            p.wait();
        }

    }

    return 0;
}
int main(int argc, const char **argv)
{
    zmq_helpers::version_assert(4, 0);
    zmq_helpers::catch_signals();

    return trustwave::authenticated_scan_server::instance().run_as <::trustwave::process_type::broker>();
}
