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
using namespace trustwave;

int Start_Session::act(boost::shared_ptr<session>, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto gsact = std::dynamic_pointer_cast<local_start_session_msg>(action);
    if(!gsact) {
        res->res("Error: Internal error");
        return -1;
    }
    trustwave::credentials creds(gsact->domain, gsact->username, gsact->password, gsact->workstation);
    auto sess = boost::make_shared<trustwave::session>(gsact->remote, creds);
    if(!authenticated_scan_server::instance().sessions->add(sess)) {
        res->res("Error: Failed adding new session");
        return -1;
    }

    res->id(gsact->id());
    res->res(sess->idstr());
    return 0;
}

Dispatcher<Action_Base>::Registrator
    Start_Session::m_registrator(new Start_Session, authenticated_scan_server::instance().public_dispatcher);
