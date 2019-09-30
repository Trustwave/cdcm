//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_file.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 May 2019
// Comments:

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <unistd.h>
#include <sstream>
#include <iomanip>
#include <string>

#include "clients/smb/smb_downloader_client.hpp"
#include "get_file.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
using namespace trustwave;

int SMB_Action::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_get_file_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->param);
    std::string tmp_name("/tmp/" + sess->idstr() + "-" + action->id());
    trustwave::smb_downloader_client rc;
    if (!rc.download(base.c_str(), "", false, true, tmp_name.c_str())) {
        res->res("Error: Download Failed");
        return -1;
    }

    res->res(tmp_name);
    return 0;

}

Dispatcher<Action_Base>::Registrator SMB_Action::m_registrator(new SMB_Action,
                authenticated_scan_server::instance().public_dispatcher);
