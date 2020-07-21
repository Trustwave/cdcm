//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_share_permissions.cpp
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
#include "get_share_permissions.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../lsa_client.hpp"
#include "../../utils/security_descriptor_utils.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::Get_Share_Permissions_Action;
using action_status = trustwave::Action_Base::action_status;

action_status Get_Share_Permissions_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                                std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return action_status::FAILED;
    }

    auto gnpact = std::dynamic_pointer_cast<lsa_action_get_share_permissions_msg>(action);
    if(!gnpact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: Internal error");
        return action_status::FAILED;
    }
    if(gnpact->share_name_.empty()) {
        res->res("Error: key is mandatory");
        return action_status::FAILED;
    }
    struct my_frame {
        void* f_;
        my_frame(): f_(talloc_stackframe()) { }
        ~my_frame() { talloc_free(f_); }
    } ff;
    {
        auto c = trustwave::lsa_client();

        result r = c.connect(*sess, gnpact->share_name_);
        if(!std::get<0>(r)) {
            AU_LOG_DEBUG("Failed connecting to %s share: %s err: %s", sess->remote().c_str(), gnpact->share_name_.c_str(),
                         win_errstr(std::get<1>(r)));

            res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));
            return action_status::FAILED;
        }

        trustwave::sd_utils::Security_Descriptor_str sd;

        auto ret = c.get_sd("", trustwave::sd_utils::entity_type::SHARE, sd);
        if(std::get<0>(ret)) { res->res(sd); }
        else {
            auto status = werror_to_ntstatus(std::get<1>(ret));
            AU_LOG_DEBUG("%s", nt_errstr(status));
            res->res(std::string("Error: ") + nt_errstr(status));
            return action_status::FAILED;
        }
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Get_Share_Permissions_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Get_Share_Permissions_Action>());
}