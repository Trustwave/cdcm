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
using namespace trustwave;
authenticated_scan_server::authenticated_scan_server() :
                logger_ptr(nullptr)
{
#undef uint_t
    //
    //  configuration path.
    //  -------------------
    const std::string root_conf("/home/ascohen/dev/samba_fresh/samba/trustwave");
    settings.load(root_conf + "/cdcm_settings.json");

    sessions = shared_mem_sessions_cache::get_or_create("sessions",1024*1024*10);


}







