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

#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../registry_client.hpp"
#include "../registry_value.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace tao ::json {

    template<>
    struct traits<trustwave::registry_value>:
        binding::object<TAO_JSON_BIND_REQUIRED("name", &trustwave::registry_value::name_),
                        TAO_JSON_BIND_REQUIRED("type", &trustwave::registry_value::type_),
                        TAO_JSON_BIND_REQUIRED("value", &trustwave::registry_value::value_)> {
    };
    template<>
    struct traits<trustwave::sub_key>:
        binding::object<TAO_JSON_BIND_REQUIRED("name", &trustwave::sub_key::name_),
                        TAO_JSON_BIND_REQUIRED("class_name", &trustwave::sub_key::class_name_),
                        TAO_JSON_BIND_REQUIRED("last_modified", &trustwave::sub_key::last_modified_)> {
    };

    template<>
    struct traits<trustwave::enum_key>:
        binding::object<TAO_JSON_BIND_REQUIRED("sub_keys", &trustwave::enum_key::sub_keys_),
                        TAO_JSON_BIND_REQUIRED("registry_values", &trustwave::enum_key::registry_values_)> {
    };
} // namespace tao::json

using trustwave::Enumerate_Key_Action;
using action_status = trustwave::Action_Base::action_status;
action_status Enumerate_Key_Action::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                                        std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return action_status::FAILED;
    }

    auto c = trustwave::registry_client();

    auto ekact = std::dynamic_pointer_cast<reg_action_enum_key_msg>(action);
    if(!ekact) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->res("Error: Internal error");
        return action_status::FAILED;
    }
    if( ekact->key_.empty())
    {
        res->res("Error: key is mandatory");
        return action_status::FAILED;
    }
    result r = c.connect(*sess);
    if(!std::get<0>(r)) {
        AU_LOG_DEBUG("Failed connecting to %s err: ", sess->remote().c_str(), win_errstr(std::get<1>(r)));
        if(werr_pipe_busy == std::get<1>(r).w) {
            res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));
            return action_status::POSTPONED;
        }
        res->res(std::string("Error: ") + std::string(win_errstr(std::get<1>(r))));
        return action_status::FAILED;
    }

    trustwave::enum_key ek{};
    auto ret = c.enumerate_key(ekact->key_, ek);
    if(std::get<0>(ret)) {
        res->res(ek);
    }
    else {
        auto status = werror_to_ntstatus(std::get<1>(ret));
        AU_LOG_DEBUG("%s", nt_errstr(status));
        res->res(std::string("Error: ") + nt_errstr(status));
        return action_status::FAILED;
    }
    return action_status::SUCCEEDED;
}

// instance of the our plugin
static std::shared_ptr<Enumerate_Key_Action> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<Enumerate_Key_Action>());
}