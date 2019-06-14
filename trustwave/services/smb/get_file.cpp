/*
 * smb_action.cpp
 *
 *  Created on: May 15, 2019
 *      Author: root
 */
#include <unistd.h>
#include <sstream>
#include <iomanip>

#include "protocol/msg_types.hpp"
#include "../../authenticated_scan_server.hpp"
#include "../clients/smb/smb_downloader_client.hpp"
#include "get_file.hpp"
using namespace trustwave;

int SMB_Action::act(const header& header,std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    session sess = authenticated_scan_server::instance().sessions.get_session_by_id(header.session_id);
    if (sess.id().is_nil()) {
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast< smb_get_file_msg >( action );
    std::string base("smb://");
    base.append(sess.remote()).append("/").append(smb_action->param);
    std::string tmp_name("/tmp/"+header.session_id+"-"+action->id());
    trustwave::smb_downloader_client rc;
    res->id(action->id());
    if (!rc.download(sess, base.c_str(), "", false, true, tmp_name.c_str())) {
        return -1;
    }

res->res(tmp_name);
    return 0;

}

Dispatcher<Action_Base>::Registrator SMB_Action::m_registrator(new SMB_Action,authenticated_scan_server::instance().public_dispatcher);
