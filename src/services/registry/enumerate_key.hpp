//===========================================================================
// Trustwave ltd. @{SRCH}
//								enumerate_key.hpp
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

#ifndef TRUSTWAVE_SERVICES_REGISTRY_ENUMERATE_KEY_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_ENUMERATE_KEY_HPP_

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "registry_action.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {

class Enumerate_Key_Action: public Registry_Action
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Enumerate_Key_Action() :
        Registry_Action("enumerate", "enumerate")
    {
    }

    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
};

}
#endif /* TRUSTWAVE_SERVICES_REGISTRY_ENUMERATE_KEY_HPP_ */
