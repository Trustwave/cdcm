//===========================================================================
// Trustwave ltd. @{SRCH}
//								registry_action.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 5 Aug 2019
// Comments: 

#ifndef TRUSTWAVE_SERVICES_REGISTRY_REGISTRY_ACTION_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_REGISTRY_ACTION_HPP_
//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "../../../../common/action.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {
class registry_client;
class session;
class Registry_Action: public Action_Base
{

public:
    Registry_Action(const std::string &name, const std::string &command) :
                    Action_Base(name, command)
    {
        std::cerr<<"Registry_Action Ctor"<<std::endl;
    }

    std::shared_ptr<registry_client> client(boost::shared_ptr<session>,
                    std::shared_ptr <result_msg> res);
};
}
#endif /* TRUSTWAVE_SERVICES_REGISTRY_REGISTRY_ACTION_HPP_ */
