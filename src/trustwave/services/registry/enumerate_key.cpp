//===========================================================================
// Trustwave ltd. @{SRCH}
//								enumerate_key.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 4 Jul 2019
// Comments: 

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "enumerate_key.hpp"

#include "../../3rdparty/json/include/tao/json.hpp"
#include "../../3rdparty/json/include/tao/json/contrib/traits.hpp"
#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../clients/registry/registry_client.hpp"
#include "../clients/registry/registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace tao {
namespace json {

template<>
struct traits<trustwave::registry_value> : binding::object<
TAO_JSON_BIND_REQUIRED ("name", &trustwave::registry_value::name_),
TAO_JSON_BIND_REQUIRED ("type", &trustwave::registry_value::type_),
TAO_JSON_BIND_REQUIRED ("value", &trustwave::registry_value::value_)>
{
};
template<>
struct traits<trustwave::sub_key> : binding::object<
TAO_JSON_BIND_REQUIRED ("name", &trustwave::sub_key::name_),
TAO_JSON_BIND_REQUIRED ("class_name", &trustwave::sub_key::class_name_),
TAO_JSON_BIND_REQUIRED ("last_modified", &trustwave::sub_key::last_modified_)>
{
};

template<>
struct traits<trustwave::enum_key> : binding::object<
TAO_JSON_BIND_REQUIRED ("sub_keyes", &trustwave::enum_key::sub_keyes_),
TAO_JSON_BIND_REQUIRED ("registry_values", &trustwave::enum_key::registry_values_)>
{
};
}
}

using namespace trustwave;

int Enumerate_Key_Action::act(const header& header, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    res->id(action->id());
    AU_LOG_DEBUG("About to look for  %s", header.session_id.c_str());
 //   authenticated_scan_server::instance().sessions.dump_by_time();
    auto sess = authenticated_scan_server::instance().sessions->get_session_by<shared_mem_sessions_cache::id>(header.session_id);
    if (sess->id().is_nil()) {
        AU_LOG_DEBUG("Session %s Not Found ERROR", header.session_id.c_str());
        res->res("Session Not Found ERROR");
        return -1;
    }

    auto ekact = std::dynamic_pointer_cast<reg_action_enum_key_msg>(action);
    if (!ekact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Bad message");
        return -1;
    }
    bool client_owend_by_me=false;
    auto c = std::dynamic_pointer_cast<trustwave::registry_client>(sess->get_client<trustwave::registry_client>(0));
    if (!c){

        c = std::make_shared<trustwave::registry_client>();
        if (!c) {
            AU_LOG_ERROR("Failed dynamic cast");
            res->res("Error");
            return -1;
        }
        client_owend_by_me = true;

    }
    struct loadparm_context *lp_ctx = ::loadparm_init_global(false);
    if (!c->connect(*sess, lp_ctx)) {
        AU_LOG_DEBUG("Failed connecting to ",sess->remote().c_str());
        res->res("Failed to connect");
        if(!client_owend_by_me)
        {
            sess->client_done(0);
        }
        return -1;
    }
    trustwave::enum_key ek;
    c->enumerate_key(ekact->key_, ek);

    const tao::json::value v1 = ek;

    res->res(to_string(v1, 2));
    if(!client_owend_by_me)
            {
                sess->client_done(0);
            }
    return 0;

}

Dispatcher<Action_Base>::Registrator Enumerate_Key_Action::m_registrator(new Enumerate_Key_Action,
                authenticated_scan_server::instance().public_dispatcher);

