//===========================================================================
// Trustwave ltd. @{SRCH}
//								authenticated_scan_server.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 30 Jun 2019
// Comments: 
#include "authenticated_scan_server.hpp"
thread_local auto log_source = ::trustwave::logger::broker;
using namespace trustwave;
authenticated_scan_server::authenticated_scan_server() :
                logger_ptr(nullptr)
{
#undef uint_t
    //
    //  configuration path.
    //  -------------------
    const std::string root_conf("/home/ascohen/dev/samba_fresh/samba/trustwave");
    if (!Initialize(logger_ptr, root_conf, ::trustwave::logger::broker)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    // ::trustwave::LoggerSource::instance()->set_source(::trustwave::logger::broker);
    zmq_helpers::version_assert(4, 0);
    zmq_helpers::catch_signals();

}

void authenticated_scan_server::run()
{
    zmq::context_t ctx(1);
    std::thread t3(message_broker::th_func, std::ref(ctx));
    std::vector<std::thread> tp;

    for (unsigned int i = 0; i < /*std::thread::hardware_concurrency() - 2*/1; ++i)        //context+broker
                    {
        tp.push_back(
                        std::move(
                                        std::thread(&message_worker::th_func,
                                                        new message_worker(std::ref(ctx), "inproc://broker"))));

    }

    t3.join();
    if (zmq_helpers::interrupted)
        printf("W: interrupt received, shutting down...\n");

}
