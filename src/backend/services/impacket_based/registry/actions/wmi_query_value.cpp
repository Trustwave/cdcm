//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_query_value.cpp
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
#include "wmi_query_value.hpp"

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../wmi_registry_client.hpp"
#include "registry_value.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::WMI_Query_Value_Action;
using action_status = trustwave::Action_Base::action_status;

action_status WMI_Query_Value_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                          std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto c = trustwave::wmi_registry_client();

    auto qvact = std::dynamic_pointer_cast<wmi_reg_action_query_value_msg>(action);
    if(!qvact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if(qvact->key_.empty()) {
        res->set_response_for_error(CDCM_ERROR::KEY_IS_MANDATORY);
        return action_status::FAILED;
    }
    auto r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s", sess->remote().c_str());
        res->set_response_for_error(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET);
        return action_status::FAILED;
    }

    trustwave::registry_value rv;
    r = c.key_get_value_by_name(qvact->key_, qvact->value_, rv);
    if(std::get<0>(r)) {
        if(rv.value().empty()) {
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                                "Value is empty");
            AU_LOG_ERROR("Error: Value is empty");
        }
        else {
            res->set_response_for_success(rv.value());
            AU_LOG_INFO(rv.value().c_str());
        }
        return action_status::SUCCEEDED;
    }
    res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                        std::get<1>(r).empty() ? "Unknown error" : std::get<1>(r));
    return action_status::FAILED;
}

// instance of the our plugin
static std::shared_ptr<WMI_Query_Value_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<WMI_Query_Value_Action>());
}