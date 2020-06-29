//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														start_session.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 20 May 2019
// Comments:

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "start_session.hpp"
#include <boost/smart_ptr/make_shared.hpp>
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
using trustwave::Start_Session;
using action_status = trustwave::Action_Base::action_status;
action_status
Start_Session::act(boost::shared_ptr<session>, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto gsact = std::dynamic_pointer_cast<local_start_session_msg>(action);
    if(!gsact) {
        res->res("Error: Internal error"); //rotem: add error code
        return Action_Base::action_status::FAILED;
    }
    if(gsact->remote.empty()  ||
       gsact->username.empty()
       )
    {
        res->res("Error: remote and username are mandatory"); //rotem: add error code
        return Action_Base::action_status::FAILED;
    }
    trustwave::credentials creds(gsact->domain, gsact->username, gsact->password, gsact->workstation);
    auto sess = boost::make_shared<trustwave::session>(gsact->remote, creds);
    if(!authenticated_scan_server::instance().sessions->add(sess)) {
        res->res("Error: Failed adding new session"); //rotem: add error code
        return Action_Base::action_status::FAILED;
    }

    res->id(gsact->id());
    res->res(sess->idstr()); //rotem: add error code
    return Action_Base::action_status::SUCCEEDED;
}

trustwave::Dispatcher<trustwave::Action_Base>::Registrator
    Start_Session::m_registrator(new Start_Session, authenticated_scan_server::instance().public_dispatcher());
