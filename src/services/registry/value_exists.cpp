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
#include "../../clients/registry/registry_client.hpp"
#include "../../clients/registry/registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using namespace trustwave;

int Value_Exists_Action::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())){
        res->res("Error: Session not found");
        return -1;
    }
    auto c = client(sess, res);

    if (!c){
        return -1;
    }

    AU_LOG_ERROR("About to cast to %s",action->name().c_str());
    auto veact = std::dynamic_pointer_cast<reg_action_value_exists_msg>(action);
    if (!veact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: internal error");
        return -1;
    }
    result r=c->connect(*sess);
    if (!std::get <0>(r)){
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(),win_errstr(std::get <1>(r)));
        res->res(std::string("Error: ")+std::string(win_errstr(std::get <1>(r))));
        return -1;
    }
    if (!std::get<0>(c->open_key(veact->key_.c_str()))) {
        AU_LOG_DEBUG("Failed opening  %s",veact->key_.c_str());
        res->res("False");
      //  res->res("Key doesn't exist");
        return -1;
    }
    trustwave::registry_value rv;
    if (!std::get<0>(c->key_get_value_by_name(veact->value_.c_str(), rv))) {
        AU_LOG_DEBUG("Failed getting value %s",veact->value_.c_str());
        res->res("False");
    }
    else {
        res->res("True");
    }
    return 0;

}

Dispatcher<Action_Base>::Registrator Value_Exists_Action::m_registrator(new Value_Exists_Action,
                authenticated_scan_server::instance().public_dispatcher);
