//===========================================================================
// Trustwave ltd. @{SRCH}
//								value_exists.cpp
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
//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "value_exists.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client.hpp"
#include "registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::Value_Exists_Action;
using action_status = trustwave::Action_Base::action_status;
action_status Value_Exists_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                       std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto c = trustwave::registry_client();

    AU_LOG_DEBUG("About to cast to %s", action->name().c_str());
    auto veact = std::dynamic_pointer_cast<reg_action_value_exists_msg>(action);
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
    result r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(), win_errstr(std::get<1>(r)));
        if(werr_pipe_busy == std::get<1>(r).w) {
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))) );
            return action_status::POSTPONED;
        }
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))));
        return action_status::FAILED;
    }
    if(!std::get<0>(c.open_key(veact->key_.c_str()))) {
        AU_LOG_DEBUG("Failed opening  %s", veact->key_.c_str());
        res->set_response_for_success("False");
        return action_status::SUCCEEDED;
    }
    trustwave::registry_value rv;
    if(!std::get<0>(c.key_get_value_by_name(veact->value_.c_str(), rv))) {
        AU_LOG_DEBUG("Failed getting value %s", veact->value_.c_str());
        res->set_response_for_success("False");
    }
    else {
        res->set_response_for_success("True");
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Value_Exists_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Value_Exists_Action>());
}
