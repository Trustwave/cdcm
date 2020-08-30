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
std::unique_ptr<trustwave::ILogger> logger_ptr_u;

template<> int trustwave::authenticated_scan_server::run_as<::trustwave::process_type::supervisor>(size_t)
{
    boost::asio::io_service ios;
    namespace bp = boost::process;
    LoggerSource::instance()->set_source(::trustwave::logger::supervisor);
    if(!Initialize(logger_ptr_u, conf_root)) {
        std::cerr << "failed to initialize the logger!!!" << std::endl;
        abort();
    }
    logger_ptr_ = logger_ptr_u.get();
    supervisor sv(ios);
    AU_LOG_INFO("%s",conf_->to_string().c_str());
    sv.run();
    ios.run();

    return 0;
}
int main(int, const char**)
{
    int rc =  trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::supervisor>();
    logger_ptr_u.reset();
    return rc;
}
