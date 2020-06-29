//===========================================================================
// Trustwave ltd. @{SRCH}
//								close_session.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 4 Jul 2019
// Comments:

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "close_session.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"

using trustwave::Close_Session;
using action_status = trustwave::Action_Base::action_status;

action_status
Close_Session::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> am, std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_resp_code(trustwave::resp_code({3,666}));
        res->res("Error: Session not found"); //error type B
        return action_status::FAILED;
    }

    authenticated_scan_server::instance().sessions->remove_by_id(sess->idstr());
    res->id(am->id());
    res->res("Session closed"); //rotem: add error code
    return action_status::SUCCEEDED;
}

trustwave::Dispatcher<trustwave::Action_Base>::Registrator
    Close_Session::m_registrator(new Close_Session, authenticated_scan_server::instance().public_dispatcher());
