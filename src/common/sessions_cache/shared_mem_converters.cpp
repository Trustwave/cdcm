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

trustwave::sp_session_t trustwave::session_converter::convert(const shared_mem_session &sm)
{
    sp_session_t rv = boost::make_shared <session>();
    if (!rv->id(std::string(sm.uuid_.c_str(), static_cast<size_t>(sm.uuid_.length())))){
        return sp_session_t();
    }
    rv->remote(std::string(sm.remote_.c_str(),static_cast<size_t>(sm.remote_.length())));
    rv->creds(std::string(sm.creds_.domain_.c_str(),static_cast<size_t>( sm.creds_.domain_.length())),
                    std::string(sm.creds_.username_.c_str(), static_cast<size_t>( sm.creds_.username_.length())),
                    std::string(sm.creds_.password_.c_str(), static_cast<size_t>( sm.creds_.password_.length())),
                    std::string(sm.creds_.workstation_.c_str(), static_cast<size_t>( sm.creds_.workstation_.length())));
    return rv;
}

trustwave::shared_mem_session trustwave::session_converter::convert(const sp_session_t &session, const void_allocator &va)
{
    shared_mem_session rv(va);
    auto idstr = session->idstr();
    rv.uuid_ = String(idstr.c_str(), idstr.length(), va);
    auto remote = session->remote();
    rv.remote_ = String(remote.c_str(), remote.length(), va);
    credentials c = session->creds();
    rv.creds_.domain_ = String(c.domain().c_str(),c.domain().length(), va);
    rv.creds_.username_ = String(c.username().c_str(),c.username().length(), va);
    rv.creds_.password_ = String(c.password().c_str(),c.password().length(), va);
    rv.creds_.workstation_ = String(c.workstation().c_str(),c.workstation().length(), va);
    return rv;
}

