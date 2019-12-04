//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_exists.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/28/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <string>

#include "../../clients/smb/smb_client.hpp"
#include "file_exists.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../../backend/utils/pe_context.hpp"
using namespace trustwave;

int SMB_File_Exists::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_file_exists_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->param);
    std::string tmp_name(authenticated_scan_server::instance().settings.downloaded_files_path_+"/" + sess->idstr() + "-" + action->id());
    trustwave::smb_client rc;
    auto connect_res = rc.connect(base.c_str());
    if(!connect_res.first)
    {
        if(connect_res.second == EEXIST)
            res->res(std::string("False"));
        else
            res->res(std::string("Error: " )+std::string((std::strerror(connect_res.second))));
    } else
    {
        res->res(std::string("True"));
    }
    return 0;

}

Dispatcher<Action_Base>::Registrator SMB_File_Exists::m_registrator(new SMB_File_Exists,
                                                                      authenticated_scan_server::instance().public_dispatcher);
