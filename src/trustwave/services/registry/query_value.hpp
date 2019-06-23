//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														query_value.hpp
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

#ifndef TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../../misc/action.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

class Query_Value_Action: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Query_Value_Action() :
                    Action_Base("query_value", "query_value")
    {
    }

    virtual int act(const header& header, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};

}

#endif /* TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_ */
