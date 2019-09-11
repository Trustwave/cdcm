//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_os.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/3/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "get_os.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../../clients/registry/registry_client.hpp"
#include "../../clients/registry/registry_value.hpp"
#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
using namespace trustwave;
using namespace std::string_literals;
static constexpr std::array<const std::string_view,5> values{"CurrentBuild", "CurrentVersion", "ProductName", "ReleaseId", "UBR"};

int GetOS_Action::act(boost::shared_ptr <session> sess, std::shared_ptr <action_msg> action, std::shared_ptr <result_msg> res)
{
    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }
    auto c = client(sess, res);

    if (!c){
        return -1;
    }
    auto qvact = std::dynamic_pointer_cast <reg_action_get_os_msg>(action);
    if (!qvact){
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: internal error");
        return -1;
    }

    if (!c->connect(*sess)){
        AU_LOG_DEBUG("Failed connecting to %s", sess->remote().c_str());
        res->res("Error: Failed to connect");
        return -1;
    }
    static constexpr std::string_view  key="SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion";
    if (!std::get <0>(c->open_key(key.data()))){
        AU_LOG_DEBUG("Failed opening  %s", key.data());
        res->res("Failed to open key");
    }
    std::map<std::string,std::string> results_map;
    for (auto val:values)
    {
        trustwave::registry_value rv;
        c->key_get_value_by_name(val.data(), rv);
        AU_LOG_INFO("%s: %s",val.data(),rv.value().c_str());
        results_map.insert({val.data(), rv.value()});
    }
    const tao::json::value map_value = results_map;
    res->res(to_string(map_value));
    return 0;

}

Dispatcher <Action_Base>::Registrator GetOS_Action::m_registrator(new GetOS_Action,
                                                                        authenticated_scan_server::instance().public_dispatcher);
