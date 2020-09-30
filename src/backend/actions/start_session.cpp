//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														start_session.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 20 May 2019
// Comments:

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "start_session.hpp"
#include <boost/smart_ptr/make_shared.hpp>
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
using trustwave::Start_Session;
using action_status = trustwave::Action_Base::action_status;
struct auth_res{
    std::string protocol;
    bool authenticated;
};
struct res1{
    std::string session_id;
    std::vector<auth_res> p;

};
namespace tao ::json {

    template<>
    struct traits<auth_res>:
        binding::object<TAO_JSON_BIND_REQUIRED("protocol", &auth_res::protocol),
            TAO_JSON_BIND_REQUIRED("authenticated", &auth_res::authenticated)
        > {
    };
    template<>
    struct traits<res1>:
        binding::object<TAO_JSON_BIND_REQUIRED("session_id", &res1::session_id),
                        TAO_JSON_BIND_REQUIRED("results", &res1::p)
                        > {
    };
}
action_status
Start_Session::act(boost::shared_ptr<session>, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto gsact = std::dynamic_pointer_cast<local_start_session_msg>(action);
    if(!gsact) {
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return Action_Base::action_status::FAILED;
    }
    if(gsact->remote.empty() // ||gsact->username.empty()
       )
    {
        res->set_response_for_error(CDCM_ERROR::REMOTE_AND_USERNAME_ARE_MANDATORY);
        return Action_Base::action_status::FAILED;
    }
    auto sess = boost::make_shared<trustwave::session>(gsact->remote);
    std::vector<std::string> to_authenticate;
    res1 resf;
    for(auto pc:gsact->protocol_credentials.value())
    {
        sess->creds(pc.protocol,pc.domain,pc.username,pc.password,pc.workstation);
        if(pc.authenticate)
        {
            to_authenticate.push_back(pc.protocol);
        }
        else
        {
            resf.p.push_back({pc.protocol, false});
        }
    }

    for(auto c:to_authenticate)
    {
        auto auth_act = authenticated_scan_server::instance().public_dispatcher().find(c+"_authenticate");
        auto action_result = std::make_shared<result_msg>();
        auth_act->act(sess, nullptr,action_result);
        if(action_result->result_.as<std::string>()=="true")
        {
            resf.p.push_back({c,true});
        }
        else {
            resf.p.push_back({c,false});
        }

    }
    if(!authenticated_scan_server::instance().sessions->add(sess)) {
        res->set_response_for_error(CDCM_ERROR::FAILED_ADDING_NEW_SESSION);
        return Action_Base::action_status::FAILED;
    }

    resf.session_id =sess->idstr();
    res->id(gsact->id());
    res->set_response_for_success(resf);
    return Action_Base::action_status::SUCCEEDED;
}

trustwave::Dispatcher<trustwave::Action_Base>::Registrator
    Start_Session::m_registrator(new Start_Session, authenticated_scan_server::instance().public_dispatcher());
