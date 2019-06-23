//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_file.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 May 2019
// Comments:

#ifndef TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_
#define TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../../misc/action.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

class SMB_Action: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    SMB_Action() :
                    Action_Base("get_file", "get_file")
    {
    }

    virtual int act(const header& header, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};

}

#endif /* TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_ */
