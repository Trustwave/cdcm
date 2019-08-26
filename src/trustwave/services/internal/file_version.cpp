//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_version.cpp
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
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../internal/file_version.hpp"

#include <boost/filesystem.hpp>

#include "../../common/protocol/msg_types.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using namespace trustwave;
using namespace trustwave::non_public;

extern "C" {
int get_ver(const char* path, char* res);
}

int Get_File_Version::act(boost::shared_ptr <session> , std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto fsact = std::dynamic_pointer_cast<local_get_file_version_msg>(action);
    res->res_.resize(256, 0);

    if (-1 == get_ver(fsact->param.c_str(), std::addressof(res->res_[0]))) {
        return -1;
    }
    res->res_.resize(strlen(res->res_.c_str()));
    return 0;

}

Dispatcher<Action_Base>::Registrator Get_File_Version::m_registrator(new Get_File_Version,
                authenticated_scan_server::instance().prv_dispatcher);
