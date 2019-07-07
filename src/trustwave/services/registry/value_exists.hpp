//===========================================================================
// Trustwave ltd. @{SRCH}
//								value_exists.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 7 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_SERVICES_REGISTRY_VALUE_EXISTS_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_VALUE_EXISTS_HPP_

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "../../misc/action.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {

class Value_Exists_Action: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Value_Exists_Action() :
                    Action_Base("value_exists", "value_exists")
    {
    }

    virtual int act(const header& header, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};
}
#endif /* TRUSTWAVE_SERVICES_REGISTRY_VALUE_EXISTS_HPP_ */
