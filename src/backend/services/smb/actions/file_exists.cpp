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

#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "pe_context.hpp"
using trustwave::SMB_File_Exists;
using action_status = trustwave::Action_Base::action_status;
action_status SMB_File_Exists::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                   std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return action_status::FAILED;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_file_exists_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->param);
    trustwave::smb_client rc;
    auto connect_res = rc.open_file(base.c_str());
    if(!connect_res.first) {
        AU_LOG_DEBUG("got smb error: %i - %s", connect_res.second, std::strerror(connect_res.second));

        if(connect_res.second == ENODEV || connect_res.second == ENOTDIR || connect_res.second == ENOENT) {
            res->res(std::string("False"));
        }
        else {
            res->res(std::string("Error: ") + std::string((std::strerror(connect_res.second))));
        }
    }
    else {
        res->res(std::string("True"));
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<SMB_File_Exists> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<SMB_File_Exists>());
}
