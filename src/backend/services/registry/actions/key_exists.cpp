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
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
using trustwave::Key_Exists_Action;
using action_status = trustwave::Action_Base::action_status;
action_status Key_Exists_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                     std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        AU_LOG_ERROR("Session not found");
        res->set_resp_code(std::move(resp_code(RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 200)));
       //res->set_resp_code(trustwave::resp_code({3,666}));
        res->res("Error: Session not found"); //error type B
        return action_status::FAILED;
    }

    auto c = trustwave::registry_client();

    auto keact = std::dynamic_pointer_cast<reg_action_key_exists_msg>(action);
    if(!keact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_resp_code(trustwave::resp_code({3,666}));
        res->res("Error: Internal error"); //error type B
        return action_status::FAILED;
    }
    if( keact->key_.empty()
        )
    {
        res->set_resp_code(trustwave::resp_code({2,666}));
        res->res("Error: key is mandatory");  //error type A
        return action_status::FAILED;
    }
    result r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(), win_errstr(std::get<1>(r)));
        if( werr_pipe_busy == std::get<1>(r).w) {
            res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));  //error type C //rotem: add error code
            return action_status::POSTPONED;
        }
        res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));  //error type C //rotem: add error code
        return action_status::FAILED;
    }
    if(!std::get<0>(c.open_key(keact->key_.c_str()))) {
        AU_LOG_DEBUG("Failed opening  %s", keact->key_.c_str());  //error type C //rotem: add error code
        res->res("False");
    }
    else {
        res->res("True"); //rotem: add error code
    }

    return action_status::SUCCEEDED;
}
//
// Dispatcher <Action_Base>::Registrator Key_Exists_Action::m_registrator(new Key_Exists_Action,
//                authenticated_scan_server::instance().public_dispatcher_);

// instance of the our plugin
static std::shared_ptr<Key_Exists_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Key_Exists_Action>());
}