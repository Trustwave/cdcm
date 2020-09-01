//===========================================================================
// Trustwave ltd. @{SRCH}
//								session_converter.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 Jul 2019
// Comments:

//===================================================================
//                          Include files
//===================================================================
#include "shared_mem_converters.hpp"
#include "shared_mem_session.hpp"
#include "../session.hpp"
#include <boost/make_shared.hpp>

trustwave::sp_session_t trustwave::session_converter::convert(const shared_mem_session& sm)
{
    sp_session_t rv = boost::make_shared<session>();
    if(!rv->id(std::string(sm.uuid_.c_str(), static_cast<size_t>(sm.uuid_.length())))) {
        return sp_session_t();
    }
    rv->remote(std::string(sm.remote_.c_str(), static_cast<size_t>(sm.remote_.length())));
    for(auto& cr:sm.creds_) {
        rv->creds(std::string(cr.first.c_str(), static_cast<size_t>(cr.first.length())),
                  std::string(cr.second.domain_.c_str(), static_cast<size_t>(cr.second.domain_.length())),
                  std::string(cr.second.username_.c_str(), static_cast<size_t>(cr.second.username_.length())),
                  std::string(cr.second.password_.c_str(), static_cast<size_t>(cr.second.password_.length())),
                  std::string(cr.second.workstation_.c_str(), static_cast<size_t>(cr.second.workstation_.length())));
    }
    return rv;
}

trustwave::shared_mem_session
trustwave::session_converter::convert(const sp_session_t& session, const void_allocator& va)
{
    shared_mem_session rv(va);
    auto idstr = session->idstr();
    rv.uuid_ = String(idstr.c_str(), idstr.length(), va);
    auto remote = session->remote();
    rv.remote_ = String(remote.c_str(), remote.length(), va);
    auto creds_map = session->creds();
    for(auto c:creds_map) {
        shared_mem_credentials c_new(va);
        c_new.domain_ = String(c.second.domain().c_str(), c.second.domain().length(), va);
        c_new.username_ = String(c.second.username().c_str(), c.second.username().length(), va);
        c_new.password_ = String(c.second.password().c_str(), c.second.password().length(), va);
        c_new.workstation_ = String(c.second.workstation().c_str(), c.second.workstation().length(), va);
        rv.creds_.insert(std::make_pair(String(c.first.c_str(), c.first.length(), va),std::move(c_new)));

    }
    return rv;
}
