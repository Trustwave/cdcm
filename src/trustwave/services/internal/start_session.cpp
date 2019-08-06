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
#include "../internal/start_session.hpp"
#include <boost/smart_ptr/make_shared.hpp>
#include "../../common/session.hpp"
#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
using namespace trustwave;

int Start_Session::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    std::cout<<"In start session"<<std::endl;
    auto gsact = std::dynamic_pointer_cast<local_start_session_msg>(action);
    trustwave::credentials creds(gsact->domain, gsact->username, gsact->password, gsact->workstation);
    auto s=boost::make_shared<trustwave::session>(gsact->remote, creds);
    authenticated_scan_server::instance().sessions->add(s);
    res->res(s->idstr());
    return 0;

}

Dispatcher<Action_Base>::Registrator Start_Session::m_registrator(new Start_Session,
                authenticated_scan_server::instance().public_dispatcher);
