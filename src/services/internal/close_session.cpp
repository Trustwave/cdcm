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
#include "../../common/session.hpp"
#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"

using namespace trustwave;

int Close_Session::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> , std::shared_ptr<result_msg> res)
{
    std::cout<<"In Close_Session"<<std::endl;

    if (!sess || (sess && sess->id().is_nil())) {
         res->res("Error: Session Not Found");
         return -1;
     }

    authenticated_scan_server::instance().sessions->remove_by_id(sess->idstr());
    res->res("session closed");
    return 0;

}

Dispatcher<Action_Base>::Registrator Close_Session::m_registrator(new Close_Session,
                authenticated_scan_server::instance().public_dispatcher);
