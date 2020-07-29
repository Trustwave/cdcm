//
// Created by rfrenkel on 6/9/2020.
//

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "enumerate_values.hpp"

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client.hpp"
#include "registry_value.hpp"

//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::Enumerate_Registry_Values_Action;
using action_status = trustwave::Action_Base::action_status;
action_status Enumerate_Registry_Values_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                                    std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto c = trustwave::registry_client();

    auto ekact = std::dynamic_pointer_cast<reg_action_enumerate_registry_values_msg>(action);
    if(!ekact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if(ekact->key_.empty()) {
        res->set_response_for_error(CDCM_ERROR::KEY_IS_MANDATORY);
        return action_status::FAILED;
    }
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

    trustwave::enum_key_values ek{};
    auto ret = c.enumerate_key_values(ekact->key_, ek);

    if(std::get<0>(ret)) { res->res(ek); }
    else {
        auto status = werror_to_ntstatus(std::get<1>(ret));
        AU_LOG_DEBUG("%s", nt_errstr(status));
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(ret)), nt_errstr(status) );
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Enumerate_Registry_Values_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Enumerate_Registry_Values_Action>());
}