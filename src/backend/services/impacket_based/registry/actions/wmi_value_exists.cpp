//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_value_exists.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 7/29/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "wmi_value_exists.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../wmi_registry_client.hpp"
#include "registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::WMI_Value_Exists_Action;
using action_status = trustwave::Action_Base::action_status;
action_status WMI_Value_Exists_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                       std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto c = trustwave::wmi_registry_client();

    AU_LOG_DEBUG("About to cast to %s", action->name().c_str());
    auto veact = std::dynamic_pointer_cast<wmi_reg_action_value_exists_msg>(action);
    if(!veact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if( veact->key_.empty()||veact->value_.empty())
    {
        res->set_response_for_error(CDCM_ERROR::KEY_AND_VALUE_ARE_MANDATORY);
        return action_status::FAILED;
    }
    bool r = c.connect(*sess);
    if(!r) {
        res->set_response_for_error(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET);
        return action_status::FAILED;
    }
    trustwave::registry_value rv;
    if(!c.key_get_value_by_name(veact->key_,veact->value_, rv)) {
        AU_LOG_DEBUG("Failed getting value %s", veact->value_.c_str());
        res->set_response_for_success("False");
    }
    else {
        res->set_response_for_success("True");
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<WMI_Value_Exists_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<WMI_Value_Exists_Action>());
}
