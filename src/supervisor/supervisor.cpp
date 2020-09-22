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
#include "supervisor.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"

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
std::unique_ptr<boost::process::child> trustwave::supervisor::start_broker()
{
    try {
        auto broker = std::make_unique<boost::process::child>(
                boost::process::search_path("cdcm_broker"),
                boost::process::on_exit([&, this](int, const std::error_code&) {
                    if(!trustwave::zmq_helpers::interrupted) {
                        broker_ = start_broker();
                    }
                }),
                ios_);
        return broker;
    }
    catch(std::exception& exception) {
        AU_LOG_ERROR("got exception while trying to start broker. exception: %s", exception.what());
        return nullptr;
    }
}
int main(int, const char**)
{
    std::cerr << "sessions_lock remove " <<(boost::filesystem::remove("/dev/shm/sessions_lock")?"SUCCEED":"FAILED")<<std::endl;
    std::cerr << "sessions remove " <<(boost::filesystem::remove("/dev/shm/sessions")?"SUCCEED":"FAILED")<<std::endl;
    int rc =  trustwave::authenticated_scan_server::instance().run_as<::trustwave::process_type::supervisor>();
    logger_ptr_u.reset();
    return rc;
}
