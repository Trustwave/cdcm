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
#include "../../clients/registry/registry_client.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Key_Exists_Action::act(boost::shared_ptr <session> sess, std::shared_ptr <action_msg> action,
                std::shared_ptr <result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())){
        res->res("Error: Session not found");
        return -1;
    }
    auto c = client(sess, res);

    if (!c){
        return -1;
    }

    auto keact = std::dynamic_pointer_cast <reg_action_key_exists_msg>(action);
    if (!keact){
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: internal error");
        return -1;

    }

    if (!c->connect(*sess)){
        AU_LOG_DEBUG("Failed connecting to %s", sess->remote().c_str());
        res->res("Error: Failed to connect");
        return -1;
    }
    if (!std::get <0>(c->open_key(keact->key_.c_str()))){
        AU_LOG_DEBUG("Failed opening  %s", keact->key_.c_str());
        res->res("False");
    }
    else{
        res->res("True");
    }

    return 0;

}

Dispatcher <Action_Base>::Registrator Key_Exists_Action::m_registrator(new Key_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
