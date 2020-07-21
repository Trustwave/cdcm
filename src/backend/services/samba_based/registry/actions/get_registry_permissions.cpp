//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_registry_permissions.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/17/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "lib/talloc/talloc.h"
#include "get_registry_permissions.hpp"

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client2.hpp"
#include "../../utils/security_descriptor_utils.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::Get_Registry_Permissions_Action;
using action_status = trustwave::Action_Base::action_status;

action_status Get_Registry_Permissions_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                                   std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }
    struct my_frame {
        void* f_;
        my_frame(): f_(talloc_stackframe()) { }
        ~my_frame() { talloc_free(f_); }
    } ff;
    {
        auto c = trustwave::registry_client2();

        auto grpact = std::dynamic_pointer_cast<reg_action_get_registry_permissions_msg>(action);
        if(!grpact) {
            AU_LOG_ERROR("Failed dynamic cast");
            res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
            return action_status::FAILED;
        }
        if(grpact->key_.empty()) {
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

        if(!std::get<0>(c.open_key(grpact->key_))) {
            AU_LOG_DEBUG("Failed opening  %s", grpact->key_.c_str());
            res->set_response_for_error(CDCM_ERROR::OPEN_KEY_FAILED);
            return action_status::FAILED;
        }
        trustwave::sd_utils::Security_Descriptor_str sd;

        auto ret = c.get_sd(sd);
        if(std::get<0>(ret)) {
            res->set_response_for_success(sd);
        }
        else {
            auto status = werror_to_ntstatus(std::get<1>(ret));
            AU_LOG_DEBUG("%s", nt_errstr(status));
            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(ret)), nt_errstr(status) );
            return action_status::FAILED;
        }
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Get_Registry_Permissions_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Get_Registry_Permissions_Action>());
}