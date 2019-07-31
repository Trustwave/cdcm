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
#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../clients/registry/registry_client.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Key_Exists_Action::act(const header& header, std::shared_ptr <action_msg> action, std::shared_ptr <result_msg> res)
{

    res->id(action->id());
    auto sess = authenticated_scan_server::instance().sessions->get_session_by <shared_mem_sessions_cache::id>(
                    header.session_id);
    if (sess->id().is_nil()){
        AU_LOG_DEBUG("Session %s Not Found ERROR", header.session_id.c_str());
        res->res("Session Not Found ERROR");
        return -1;
    }
    auto keact = std::dynamic_pointer_cast <reg_action_key_exists_msg>(action);
    if (!keact){
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Bad message");
        return -1;

    }
    auto c = std::dynamic_pointer_cast <trustwave::registry_client>(sess->get_client <trustwave::registry_client>(0));
    bool client_owend_by_me = false;
    if (!c){
        c = std::make_shared <trustwave::registry_client>();
        if (!c){
            AU_LOG_ERROR("Failed dynamic cast");
            res->res("Error");
            return -1;
        }
        client_owend_by_me = true;

    }
    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!c->connect(*sess, lp_ctx)){
        AU_LOG_DEBUG("Failed connecting to ",sess->remote().c_str());
        res->res("Failed to connect");
        if (!client_owend_by_me)
            sess->client_done(0);
        return -1;
    }
    if (!std::get <0>(c->open_key(keact->key_.c_str()))){
        AU_LOG_DEBUG("Failed opening  %s",keact->key_.c_str());
        res->res("False");
    }
    else{
        res->res("True");
    }
    if (!client_owend_by_me)
        sess->client_done(0);
    return 0;

}

Dispatcher <Action_Base>::Registrator Key_Exists_Action::m_registrator(new Key_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
