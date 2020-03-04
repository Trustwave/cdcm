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
#include "../registry_value.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using trustwave::Query_Value_Action;
using action_status = trustwave::Action_Base::action_status;

action_status Query_Value_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                      std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return action_status::FAILED;
    }

    auto c = trustwave::registry_client();

    auto qvact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);
    if(!qvact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: Internal error");
        return action_status::FAILED;
    }
    result r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(), win_errstr(std::get<1>(r)));
        if(WERR_PIPE_BUSY.w == std::get<1>(r).w) {
            res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));
            return action_status::POSTPONED;
        }
        res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));
        return action_status::FAILED;
    }

    if(!std::get<0>(c.open_key(qvact->key_.c_str()))) {
        AU_LOG_DEBUG("Failed opening  %s", qvact->key_.c_str());
        res->res("Error: Failed to open key");
        return action_status::FAILED;
    }
    trustwave::registry_value rv;
    c.key_get_value_by_name(qvact->value_.c_str(), rv);
    if(rv.value().empty()) {
        res->res("Error: Value is empty");
        AU_LOG_ERROR("Error: Value is empty");
    }
    else {
        res->res(rv.value());
        AU_LOG_INFO(rv.value().c_str());
    }
    return action_status::SUCCEEDED;
}

// Dispatcher <Action_Base>::Registrator Query_Value_Action::m_registrator(new Query_Value_Action,
//                authenticated_scan_server::instance().public_dispatcher_);

// instance of the our plugin
static std::shared_ptr<Query_Value_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Query_Value_Action>());
}