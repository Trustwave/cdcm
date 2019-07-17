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
#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../clients/registry/registry_client.hpp"
#include "../clients/registry/registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Value_Exists_Action::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    res->id(action->id());
    auto sess = authenticated_scan_server::instance().sessions->get_session_by<shared_mem_sessions_cache::id>(header.session_id);
    if (sess->id().is_nil()) {
        res->res("Session Not Found ERROR");
        return -1;
    }

    auto veact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);
    if (!veact) {
        res->res("Error");
        return -1;
    }
    auto c = std::dynamic_pointer_cast<trustwave::registry_client>(sess->get_client<trustwave::registry_client>(0));

    if (!c) {

        c = std::make_shared<trustwave::registry_client>();
        if (!c) {
            res->res("Error");
            return -1;
        }

    }

    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!c->connect(*sess, lp_ctx)) {
        std::cerr << "Failed to connect!!!" << std::endl;
        res->res("Failed to connect");
        return -1;
    }
    if (!std::get<0>(c->open_key(veact->key_.c_str()))) {
        std::cerr << "Failed to open key!!!" << std::endl;
        res->res("Key doesn't exist");
        return -1;
    }
    trustwave::registry_value rv;
    if (!std::get<0>(c->key_get_value_by_name(veact->value_.c_str(), rv))) {
        std::cerr << "Failed to Get value!!!" << std::endl;
        res->res("False");
    }
    else {
        res->res("True");
        std::cerr << "value Exists!!!" << std::endl;
    }
    return 0;

}

Dispatcher<Action_Base>::Registrator Value_Exists_Action::m_registrator(new Value_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
