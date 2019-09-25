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
#include "../../common/singleton_runner/authenticated_scan_server.hpp"

#include "../session.hpp"
using namespace trustwave;
authenticated_scan_server::authenticated_scan_server() :
                logger_ptr(nullptr)
{
#undef uint_t
    //
    //  configuration path.
    //  -------------------

    settings.load(std::string(conf_root) + "/cdcm_settings.json");
    sessions = shared_mem_sessions_cache::get_or_create("sessions", 1024 * 1024 * 10,settings.session_idle_time_);

}
boost::shared_ptr <session> authenticated_scan_server::get_session(const std::string& session_id)
{
    AU_LOG_DEBUG("About to look for  %s", session_id.c_str());
    std::cout << "About to look for session_id: " << session_id << std::endl;
    auto sess = sessions->get_session_by <shared_mem_sessions_cache::id>(session_id);
    if (!sess || (sess && sess->id().is_nil())){
        AU_LOG_DEBUG("Session %s Not Found ", session_id.c_str());
    }
    return sess;
}
