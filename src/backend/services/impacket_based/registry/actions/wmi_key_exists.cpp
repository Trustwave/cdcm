//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_key_exists.cpp
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
#include "wmi_key_exists.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../wmi_registry_client.hpp"
#include "registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::WMI_Key_Exists_Action;
using action_status = trustwave::Action_Base::action_status;
action_status WMI_Key_Exists_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                     std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        AU_LOG_ERROR("Session not found");
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto c = trustwave::wmi_registry_client();

    auto keact = std::dynamic_pointer_cast<wmi_reg_action_key_exists_msg>(action);
    if(!keact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if( keact->key_.empty() )
    {
        res->set_response_for_error(CDCM_ERROR::KEY_IS_MANDATORY);
        return action_status::FAILED;
    }
    bool r = c.connect(*sess);
    if(!r) {
        AU_LOG_DEBUG("Failed connecting to %s ", sess->remote().c_str());
        res->set_response_for_error(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET);
        return action_status::FAILED;
    }
    enum_key ek;
    if(!c.enumerate_key(keact->key_,ek)) {
        AU_LOG_DEBUG("Failed opening  %s", keact->key_.c_str());
        res->set_response_for_success("False");
    }
    else {
        res->set_response_for_success("True");
    }

    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<WMI_Key_Exists_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<WMI_Key_Exists_Action>());
}