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

#include "../session.hpp"
using trustwave::authenticated_scan_server;
authenticated_scan_server::authenticated_scan_server(): logger_ptr_(nullptr)
{
#undef uint_t

    auto fn = std::string(std::string(conf_root) + "/cdcm_settings.json");
    const tao::json::value v = tao::json::from_file(fn);
    conf_ = v.as<std::shared_ptr<cdcm_settings>>();
    service_conf_repository.register1(conf_);
    sessions = shared_mem_sessions_cache::get_or_create("sessions", conf_->session_cache_size_*MB , conf_->session_idle_time_);
}
boost::shared_ptr<trustwave::session> authenticated_scan_server::get_session(const std::string& session_id)
{
    AU_LOG_DEBUG("About to look for  %s", session_id.c_str());
    auto sess = sessions->get_session_by<shared_mem_sessions_cache::id>(session_id);
    if(!sess || (sess && sess->id().is_nil())) {
        AU_LOG_DEBUG("Session %s Not Found ", session_id.c_str());
    }
    return sess;
}
trustwave::Dispatcher<trustwave::Action_Base>& authenticated_scan_server::public_dispatcher()
{
    return public_dispatcher_;
}
trustwave::Dispatcher<trustwave::process_specific_object>& authenticated_scan_server::process_specific_repository()
{
    return process_specific_repository_;
}

boost::asio::io_service& authenticated_scan_server::io_context()
{
    return ios_;
}
