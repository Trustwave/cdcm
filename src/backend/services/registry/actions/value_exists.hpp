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
#include "registry_action.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {

class Value_Exists_Action: public Registry_Action
{
public:
    Value_Exists_Action() :
        Registry_Action("value_exists", "value_exists")
    {
    }

    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
};
}
#endif /* TRUSTWAVE_SERVICES_REGISTRY_VALUE_EXISTS_HPP_ */
