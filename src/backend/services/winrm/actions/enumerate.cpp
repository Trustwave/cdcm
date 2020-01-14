//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														enumerate.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 12/30/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "enumerate.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "OpenWsmanClient.h"
using namespace trustwave;

int Winrm_Enumerate_Action::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }
    OpenWsmanClient cli(sess->remote(),5985,"/wsman","http","Basic",sess->creds().username(),sess->creds().password());
    const std::string r{"http://schemas.microsoft.com/wbem/wsman/1/wmi/root/cimv2/Win32_Service"};
    std::vector<std::string> res2;
    cli.Enumerate(r,res2);
    return 0;
}

// instance of the our plugin
static std::shared_ptr<Winrm_Enumerate_Action> instance = nullptr;


// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action() {
    return instance ? instance : (instance = std::make_shared<Winrm_Enumerate_Action>());
}

