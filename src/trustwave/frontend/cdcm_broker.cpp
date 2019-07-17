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
#include "backend/message_worker.hpp"
#include "frontend/message_broker.hpp"
template<>
int trustwave::authenticated_scan_server::run_as<::trustwave::logger::broker>()
{

        zmq::context_t ctx(1);
        boost::asio::io_service ios;
        namespace bp = boost::process;
        std::vector<char> buf(4096);

        std::thread broker_thread(message_broker::th_func, std::ref(ctx));
        bp::group worker_process_group;

        for (size_t i = 0; i < /*std::thread::hardware_concurrency() - 2*/3; ++i)        //context+broker
                        {

            bp::child("/home/ascohen/dev/samba_fresh/samba/bin/cdcm_worker", std::to_string(i),
                            bp::std_out > boost::asio::buffer(buf), ios, worker_process_group);

        }
        //todo assaf add worker threads as well
        ios.run();
        broker_thread.join();

        worker_process_group.wait();

        if (zmq_helpers::interrupted)
            printf("W: interrupt received, shutting down...\n");
    return 0;
}
int main(int argc, const char **argv)
{
    zmq_helpers::version_assert(4, 0);
    zmq_helpers::catch_signals();

    return trustwave::authenticated_scan_server::instance().run_as<::trustwave::logger::broker>();
}
