//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_cdcm_info.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/8/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "get_cdcm_info.hpp"
#include <boost/smart_ptr/make_shared.hpp>
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
using trustwave::Get_CDCM_Info;
using action_status = trustwave::Action_Base::action_status;
action_status
Get_CDCM_Info::act(boost::shared_ptr<session>, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto gact = std::dynamic_pointer_cast<local_get_cdcm_info_msg>(action);
    if(!gact) {
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return Action_Base::action_status::FAILED;
    }
    res->id(gact->id());
    if(gact->what == "cdcm_version")
    {
        res->set_response_for_success(authenticated_scan_server::instance().settings()->cdcm_version_);
        return Action_Base::action_status::SUCCEEDED;
    }
    res->set_response_for_error(CDCM_ERROR::BAD_PARAMETER);
    return Action_Base::action_status::FAILED;
}

trustwave::Dispatcher<trustwave::Action_Base>::Registrator
        Get_CDCM_Info::m_registrator(new trustwave::Get_CDCM_Info, authenticated_scan_server::instance().public_dispatcher());
