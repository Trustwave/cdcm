//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_authenticate.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/31/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "wmi_authenticate.hpp"

#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../wmi_registry_client.hpp"
#include "scoped_client.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::WMI_Authenticate_Action;
using action_status = trustwave::Action_Base::action_status;

action_status WMI_Authenticate_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                            std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }


    auto ekact = std::dynamic_pointer_cast<wmi_authenticate_action_msg>(action);
    if(!ekact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }

    auto c_base = authenticated_scan_server::instance().process_specific_repository().find_as<sessions_to_clients>()->get_client<trustwave::wmi_registry_client>(sess->idstr());
    auto c = std::dynamic_pointer_cast<trustwave::wmi_registry_client>(c_base);
    scoped_client sc(c_base,sess->idstr());
    auto r = c->connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s ", sess->remote().c_str());
        res->set_response_for_success("false");
    }
    else
    {
        res->set_response_for_success("true");
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<WMI_Authenticate_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<WMI_Authenticate_Action>());
}