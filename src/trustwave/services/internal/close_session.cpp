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
#include "../internal/close_session.hpp"
#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"

using namespace trustwave;

int Close_Session::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    std::cout<<"In Close_Session"<<std::endl;


    authenticated_scan_server::instance().sessions->remove_by_id(header.session_id);
    res->id(action->id());
    res->res("session closed");
    return 0;

}

Dispatcher<Action_Base>::Registrator Close_Session::m_registrator(new Close_Session,
                authenticated_scan_server::instance().public_dispatcher);
