//===========================================================================
// Trustwave ltd. @{SRCH}
//								key_exists.hpp
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

#ifndef TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "../../common/action.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {

class Key_Exists_Action: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Key_Exists_Action() :
                    Action_Base("key_exists", "key_exists")
    {
    }

    virtual int act(const header& header, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};

}

#endif /* TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_ */
