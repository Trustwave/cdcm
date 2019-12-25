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

#include "../smb_client.hpp"
#include "file_exists.hpp"

#include "../../../../common/protocol/msg_types.hpp"
#include "../../../../common/session.hpp"
#include "../../../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../../../utils/pe_context.hpp"
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
    trustwave::smb_client rc;
    auto connect_res = rc.connect(base.c_str());
    if(!connect_res.first){
            //rotem TODO: remove this after code review
//         based on linux error https://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html and description in
//         ENOMEM  Out of memory. error no 12
//         EINVAL if an invalid parameter passed, like no file, or smbc_init not called. error no 22
//         EEXIST  pathname already exists and O_CREAT and O_EXCL were used. error no 17
//         EISDIR  pathname  refers  to  a  directory  and the access requested involved writing. error no 21
//         EACCES  The requested access to the file is not allowed. error no 13
//         ENODEV The requested share does not exist. error no 19
//         ENOTDIR A file on the path is not a directory. error no 20
//         ENOENT  A directory component in pathname does not exist. error no 02

        AU_LOG_DEBUG("got smb error: %i - %s", connect_res.second, std::strerror(connect_res.second));

        if(connect_res.second == ENODEV || connect_res.second == ENOTDIR || connect_res.second == ENOENT ) {
            res->res(std::string("False"));
        }else{
            res->res(std::string("Error: " )+std::string((std::strerror(connect_res.second))));
        }
    }else{
        res->res(std::string("True"));
    }
    return 0;
}

// instance of the our plugin
static std::shared_ptr<SMB_File_Exists> instance = nullptr;


// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action() {
    return instance ? instance : (instance = std::make_shared<SMB_File_Exists>());
}
