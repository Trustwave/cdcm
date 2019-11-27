//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_file_info.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/27/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <string>

#include "../../clients/smb/smb_client.hpp"
#include "get_file_info.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../../backend/utils/pe_context.hpp"
using namespace trustwave;

int SMB_Get_File_Info::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_get_file_info_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->param);
    std::string tmp_name(authenticated_scan_server::instance().settings.downloaded_files_path_+"/" + sess->idstr() + "-" + action->id());
    trustwave::smb_client rc;
    rc.connect(base.c_str());
    pe_context pc(rc);
    pc.parse();
    pc.showVersion();
//    if (!rc.download(base.c_str(), "", false, true, tmp_name.c_str())) {
//        res->res("Error: Download failed");
//        return -1;
//    }

    res->res(tmp_name);
    return 0;

}

Dispatcher<Action_Base>::Registrator SMB_Get_File_Info::m_registrator(new SMB_Get_File_Info,
                                                                 authenticated_scan_server::instance().public_dispatcher);
