//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														query_value.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 May 2019
// Comments:

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "query_value.hpp"

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client.hpp"
#include "registry_value.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::Query_Value_Action;
using action_status = trustwave::Action_Base::action_status;

action_status Query_Value_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                      std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto qvact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);
    if(!qvact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if( qvact->key_.empty())
    {
        res->set_response_for_error(CDCM_ERROR::KEY_IS_MANDATORY);
        return action_status::FAILED;
    }
    auto c = trustwave::registry_client();
    result r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(), win_errstr(std::get<1>(r)));
        if(werr_pipe_busy == std::get<1>(r).w) {
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))));
            return action_status::POSTPONED;
        }
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))));
        return action_status::FAILED;
    }

    r = c.open_key(qvact->key_.c_str());
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed opening  %s %s ", qvact->key_.c_str(),win_errstr(std::get<1>(r)));
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET,
                                                                W_ERROR_V(std::get<1>(r)),
                                                                std::string(win_errstr(std::get<1>(r))));
        return action_status::FAILED;
    }
    trustwave::registry_value rv;
    r = c.key_get_value_by_name(qvact->value_.c_str(), rv);
    if(!std::get<0>(r)) {
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))));
    }
    else
    {
        if(rv.value().empty()) {
            res->set_response_for_error(CDCM_ERROR::VALUE_IS_EMPTY);
            AU_LOG_ERROR("Error: Value is empty");
        }
        else {
            res->set_response_for_success(rv.value());
            AU_LOG_INFO(rv.value().c_str());
        }
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Query_Value_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Query_Value_Action>());
}