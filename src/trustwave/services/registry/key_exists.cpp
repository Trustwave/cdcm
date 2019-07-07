//===========================================================================
// Trustwave ltd. @{SRCH}
//								key_exists.cpp
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
#include "key_exists.hpp"
#include "../../authenticated_scan_server.hpp"
#include "protocol/msg_types.hpp"
#include "../clients/registry/registry_client.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Key_Exists_Action::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    res->id(action->id());
    session sess = authenticated_scan_server::instance().sessions.get_session_by_id(header.session_id);
    if (sess.id().is_nil()) {
        res->res("Session Not Found ERROR");
        return -1;
    }

    auto keact = std::dynamic_pointer_cast<reg_action_query_value_msg>(action);

    trustwave::registry_client rc;
    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!rc.connect(sess, lp_ctx)) {
        std::cerr << "Failed to connect!!!" << std::endl;
        res->res("Failed to connect");
        return -1;
    }
    if (!std::get<0>(rc.open_key(keact->key_.c_str()))) {
        std::cerr << "Failed to open key!!!" << std::endl;
        res->res("False");
    }
    else
    {
        std::cerr << "key Exists!!!" << std::endl;
        res->res("True");
    }

    return 0;

}

Dispatcher<Action_Base>::Registrator Key_Exists_Action::m_registrator(new Key_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
