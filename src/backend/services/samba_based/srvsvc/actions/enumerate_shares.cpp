//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														enumerate_shares.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/22/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "lib/talloc/talloc.h"
#include "enumerate_shares.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../srvsvc_client.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::Enumerate_Shares_Action;
using action_status = trustwave::Action_Base::action_status;

namespace tao ::json {
    template<>
    struct traits<trustwave::share_info>:
        binding::object<TAO_JSON_BIND_REQUIRED("Name", &trustwave::share_info::name_),
                        TAO_JSON_BIND_REQUIRED("Comment", &trustwave::share_info::comment_),
                        TAO_JSON_BIND_REQUIRED("Path", &trustwave::share_info::path_)

                        > {
    };

} // namespace tao::json
action_status Enumerate_Shares_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                           std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto esact = std::dynamic_pointer_cast<srvsvc_action_enumerate_shares_msg>(action);
    if(!esact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }

    struct my_frame {
        void* f_;
        my_frame(): f_(talloc_stackframe()) { }
        ~my_frame() { talloc_free(f_); }
    } ff;
    {
        auto c = trustwave::srvsvc_client();

        result r = c.connect(*sess);
        if(!std::get<0>(r)) {
            AU_LOG_DEBUG("Failed connecting to %s err: %s", sess->remote().c_str(), win_errstr(std::get<1>(r)));

            res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, W_ERROR_V(std::get<1>(r)), std::string(win_errstr(std::get<1>(r))));
            return action_status::FAILED;
        }

        std::vector<trustwave::share_info> shares_vec;
        auto ret = c.enumerate_all_shares(shares_vec);

        if(std::get<0>(ret)) {
            res->set_response_for_success(shares_vec);
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
static std::shared_ptr<Enumerate_Shares_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Enumerate_Shares_Action>());
}
