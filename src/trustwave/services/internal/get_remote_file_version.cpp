//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_remote_file_version.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 May 2019
// Comments:

#include "../internal/get_remote_file_version.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
using namespace trustwave;
int Get_Remote_File_Version::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action,
                std::shared_ptr<result_msg> res)
{
    auto spact_msg = std::dynamic_pointer_cast<single_param_action_msg>(action); //the real
    if (!spact_msg) {
        res->res("Malformed message");
        return -1;
    }
    auto gfact = authenticated_scan_server::instance().public_dispatcher.find("get_file");
    if (-1 != gfact->act(sess, std::make_shared<smb_get_file_msg>(*spact_msg), res)) {
        auto lmsg = std::make_shared<local_get_file_version_msg>(*spact_msg);
        lmsg->param = res->res();
        auto fsact = authenticated_scan_server::instance().prv_dispatcher.find("get_file_version");
        return fsact->act(sess, lmsg, res);
    }
    return -1;

}

Dispatcher<Action_Base>::Registrator Get_Remote_File_Version::m_registrator(new Get_Remote_File_Version,
                authenticated_scan_server::instance().public_dispatcher);
