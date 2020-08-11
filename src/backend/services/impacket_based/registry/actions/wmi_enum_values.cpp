//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_enum_values.cpp
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
//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "wmi_enum_values.hpp"

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../wmi_registry_client.hpp"
#include "registry_value.hpp"

//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::WMI_Enumerate_Registry_Values_Action;
using action_status = trustwave::Action_Base::action_status;
action_status WMI_Enumerate_Registry_Values_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                                    std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }
    auto c = trustwave::wmi_registry_client();
    auto ekact = std::dynamic_pointer_cast<wmi_reg_action_enumerate_registry_values_msg>(action);
    if(!ekact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if(ekact->key_.empty()) {
        res->set_response_for_error(CDCM_ERROR::KEY_IS_MANDATORY);
        return action_status::FAILED;
    }
    auto r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s ", sess->remote().c_str());
        res->set_response_for_error(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET);
        return action_status::FAILED;
    }
    bool exists = false;
    r = c.key_exists(ekact->key_, exists);
    if(std::get<0>(r)) {
        if(exists) {
            trustwave::enum_key_values ek{};
            r = c.enumerate_key_values(ekact->key_, ek);

            if(std::get<0>(r)) { res->res(ek); }
            else {
                res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                                    std::get<1>(r).empty()?"Unknown error":std::get<1>(r));
                return action_status::FAILED;
            }
            return action_status::SUCCEEDED;
        }
        else {
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                                "Key Doesn't Exist");
            return action_status::FAILED;
        }
    }
    else
    {
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                            std::get<1>(r).empty()?"Unknown error":std::get<1>(r));
        return action_status::FAILED;
    }
}

// instance of the our plugin
static std::shared_ptr<WMI_Enumerate_Registry_Values_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<WMI_Enumerate_Registry_Values_Action>());
}