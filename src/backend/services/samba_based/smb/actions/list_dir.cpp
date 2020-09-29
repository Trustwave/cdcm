//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														list_dir.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/4/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "list_dir.hpp"
#include <iomanip>
#include <boost/algorithm/string/replace.hpp>
#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../smb_client.hpp"
#include "session.hpp"
#include "wildcards.hpp"

namespace tao::json {
    template<>
    struct traits<trustwave::dirent>:
        binding::object<TAO_JSON_BIND_REQUIRED("name", &trustwave::dirent::name_),
                        TAO_JSON_BIND_REQUIRED("type", &trustwave::dirent::type_)> {
    };
} // namespace tao::json
using trustwave::SMB_List_Dir;
using action_status = trustwave::Action_Base::action_status;
action_status
SMB_List_Dir::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_list_dir_msg>(action);
    if(!smb_action) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if( smb_action->param.empty())
    {
        res->set_response_for_error(CDCM_ERROR::PATH_IS_MANDATORY);
        return action_status::FAILED;
    }
    std::string path = boost::replace_all_copy(smb_action->param, "\\", "/");
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(path);
    std::string tmp_name("/tmp/" + sess->idstr() + "-" + action->id());
    trustwave::smb_client rc;
    std::vector<trustwave::dirent> dir_entries;
    if(!rc.list_dir(base.c_str(), dir_entries)) {
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET,errno,strerror(errno) );
        return action_status::FAILED;
    }
    AU_LOG_INFO("list returned");
    if(!smb_action->pattern.empty()) {
        dir_entries.erase(
            std::remove_if(dir_entries.begin(), dir_entries.end(),
                           [&](const trustwave::dirent& fname) -> bool {
                               return !wildcards::match(
                                   cx::make_string_view(fname.name_.c_str(), fname.name_.length()),
                                   cx::make_string_view(smb_action->pattern.c_str(), smb_action->pattern.length()));
                           }),
            dir_entries.end());
    }

    res->set_response_for_success(dir_entries);
    return action_status::SUCCEEDED;
}
static std::shared_ptr<SMB_List_Dir> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<SMB_List_Dir>());
}
