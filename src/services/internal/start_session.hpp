//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														start_session.hpp
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

#ifndef TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_
#define TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../../common/action.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

class Start_Session: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Start_Session() :
                    Action_Base("start_session", "start_session",false)
    {
    }
    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
};

}

#endif /* TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_ */
