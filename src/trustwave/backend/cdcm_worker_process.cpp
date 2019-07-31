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
#include <boost/process.hpp>
#include <boost/asio.hpp>

#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/zmq_helpers.hpp"
#include "backend/message_worker.hpp"
template<>
int trustwave::authenticated_scan_server::run_as<::trustwave::logger::worker>(size_t id)
{
    const std::string root_conf("/home/ascohen/dev/samba_fresh/samba/trustwave");
    LoggerSource::instance()->set_source(::trustwave::logger::worker,id);
        if (!Initialize(logger_ptr, root_conf)) {
                std::cerr << "failed to initialize the logger!!!" << std::endl;
                abort();
            }
        std::cerr << id<<"logger initialised!!!" << std::endl;
    std::thread worker_thread(message_worker::main_func);
        worker_thread.join();
        return 0;
}
int main(int argc, char **argv)
{
    zmq_helpers::version_assert(4, 0);
    zmq_helpers::catch_signals();
    return trustwave::authenticated_scan_server::instance().run_as<::trustwave::logger::worker>(std::stoull(argv[1]));
}
