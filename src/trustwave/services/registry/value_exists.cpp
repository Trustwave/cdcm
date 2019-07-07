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
#include "../../authenticated_scan_server.hpp"
#include "protocol/msg_types.hpp"
#include "../clients/registry/registry_client.hpp"
#include "../clients/registry/registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Value_Exists_Action::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    res->id(action->id());
    session sess = authenticated_scan_server::instance().sessions.get_session_by_id(header.session_id);
    if (sess.id().is_nil()) {
        res->res("Session Not Found ERROR");
        return -1;
    }

    auto veact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);

    trustwave::registry_client rc;
    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!rc.connect(sess, lp_ctx)) {
        std::cerr << "Failed to connect!!!" << std::endl;
        res->res("Failed to connect");
        return -1;
    }
    if (!std::get<0>(rc.open_key(veact->key_.c_str()))) {
        std::cerr << "Failed to open key!!!" << std::endl;
        res->res("Key doesn't exist");
        return -1;
    }
    trustwave::registry_value rv;
    if (!std::get<0>(rc.key_get_value_by_name(veact->value_.c_str(), rv))) {
            std::cerr << "Failed to Get value!!!" << std::endl;
            res->res("False");
        }
    else{
    res->res("True");
    std::cerr << "value Exists!!!" << std::endl;
    }
        return 0;


}

Dispatcher<Action_Base>::Registrator Value_Exists_Action::m_registrator(new Value_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
