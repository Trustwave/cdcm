/*
 * file_size.cpp
 *
 *  Created on: May 23, 2019
 *      Author: root
 */

#include "file_version.hpp"

#include "../../authenticated_scan_server.hpp"
#include "protocol/msg_types.hpp"
#include <boost/filesystem.hpp>
using namespace trustwave;
using namespace trustwave::non_public;

extern "C" {
int get_ver(const char* path, char* res);
}

int Get_File_Version::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    auto fsact = std::dynamic_pointer_cast<local_get_file_version_msg>(action);
    res->res_.resize(256, 0);

    if (-1 == get_ver(fsact->param.c_str(), std::addressof(res->res_[0]))) {
        return -1;
    }
    res->res_.resize(strlen(res->res_.c_str()));
    res->id(action->id());
    return 0;

}

Dispatcher<Action_Base>::Registrator Get_File_Version::m_registrator(new Get_File_Version, authenticated_scan_server::instance().prv_dispatcher);
