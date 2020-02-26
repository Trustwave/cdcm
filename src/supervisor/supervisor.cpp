//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														supervisor.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 2/25/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <zmq.hpp>
#include <boost/asio.hpp>

#include "singleton_runner/authenticated_scan_server.hpp"
#include "supervisor.hpp"

template<> int trustwave::authenticated_scan_server::run_as<::trustwave::process_type::supervisor>(size_t)
{
    boost::asio::io_service ios;
    namespace bp = boost::process;
    LoggerSource::instance()->set_source(::trustwave::logger::supervisor);
    if(!Initialize(logger_ptr_, conf_root)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    supervisor sv(ios);
    sv.run();
    ios.run();

    return 0;
}
int main(int, const char**)
{
    return trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::supervisor>();
}
