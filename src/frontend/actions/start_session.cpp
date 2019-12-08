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
#include "../../common/session.hpp"
#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
using namespace trustwave;

int Start_Session::act(boost::shared_ptr <session> , std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto gsact = std::dynamic_pointer_cast<local_start_session_msg>(action);
    if(!gsact)
    {
        res->res("Error: Internal error");
        return -1;
    }
    trustwave::credentials creds(gsact->domain, gsact->username, gsact->password, gsact->workstation);
    auto sess=boost::make_shared<trustwave::session>(gsact->remote, creds);
    if(false == authenticated_scan_server::instance().sessions->add(sess))
    {
        res->res("Error: Failed adding new session");
        return -1;
    }
//    auto c = std::dynamic_pointer_cast <trustwave::registry_client>(sess->get_client <trustwave::registry_client>(0));
//    if (!c){
//        return -1;
//    }
//    result r=c->connect(*sess);
//    if (!std::get <0>(r)){
//        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(),win_errstr(std::get <1>(r)));
//        res->res(std::string("Error: ")+std::string(win_errstr(std::get <1>(r))));
//        if(false == authenticated_scan_server::instance().sessions->remove_by_id(sess->idstr()))
//        {
//            AU_LOG_ERROR("Failed removing session from cache: %s",sess->idstr().c_str());
//        }
//
//        return -1;
//    }
    res->id(gsact->id());
    res->res(sess->idstr());
    return 0;

}

Dispatcher<Action_Base>::Registrator Start_Session::m_registrator(new Start_Session,
                authenticated_scan_server::instance().public_dispatcher);
