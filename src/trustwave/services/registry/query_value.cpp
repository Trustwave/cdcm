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

#include "../../authenticated_scan_server.hpp"
#include "protocol/msg_types.hpp"

#include "../clients/registry/registry_client.hpp"
#include "../clients/registry/registry_value.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using namespace trustwave;

int Query_Value_Action::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    res->id(action->id());
    session sess = authenticated_scan_server::instance().sessions.get_session_by_id(header.session_id);
    if (sess.id().is_nil()) {
        return -1;
        res->res("Session Not Found ERROR");
    }

    auto qvact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);

    trustwave::registry_client rc;
    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!rc.connect(sess, lp_ctx)) {
        res->res("Failed to connect");
        return -1;
    }
    rc.open_key(qvact->key_.c_str());
    trustwave::registry_value rv;
    rc.key_get_value_by_name(qvact->value_.c_str(), rv);
    AU_LOG_INFO(rv.value().c_str());

    res->res(rv.value());
    return 0;

}

Dispatcher<Action_Base>::Registrator Query_Value_Action::m_registrator(new Query_Value_Action,
                authenticated_scan_server::instance().public_dispatcher);
