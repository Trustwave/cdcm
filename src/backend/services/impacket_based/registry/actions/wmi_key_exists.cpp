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
#include "scoped_client.hpp"

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
    auto c_base = authenticated_scan_server::instance().process_specific_repository().find_as<sessions_to_clients>()->get_client<trustwave::wmi_registry_client>(sess->idstr());
    auto c = std::dynamic_pointer_cast<trustwave::wmi_registry_client>(c_base);
    scoped_client sc(c_base,sess->idstr());
    auto r = c->connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s ", sess->remote().c_str());
        res->set_response_for_error(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET);
        return action_status::FAILED;
    }
    bool exists=false;
    r = c->key_exists(keact->key_,exists);
    if(std::get<0>(r)) {
//        AU_LOG_DEBUG("Failed opening  %s", keact->key_.c_str());
        res->set_response_for_success(exists?"True":"False");
    }
    else {
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, 0,
                                                            std::get<1>(r).empty()?"Unknown error":std::get<1>(r));
        return action_status::FAILED;
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