//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_registry_permissions.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/17/20
// Comments:
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_ACTIONS_GET_REGISTRY_PERMISSIONS_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_ACTIONS_GET_REGISTRY_PERMISSIONS_HPP

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
#include "action.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    struct reg_action_get_registry_permissions_msg: public action_msg {
        static constexpr std::string_view act_name{"get_registry_permissions"};
        reg_action_get_registry_permissions_msg(): action_msg(act_name) { }
        std::string key_;
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::reg_action_get_registry_permissions_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>,
                        TAO_JSON_BIND_REQUIRED("key", &trustwave::reg_action_get_registry_permissions_msg::key_)> {
        TAO_JSON_DEFAULT_KEY(trustwave::reg_action_get_registry_permissions_msg::act_name.data());
        template<template<typename...> class Traits>
        static trustwave::reg_action_get_registry_permissions_msg as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::reg_action_get_registry_permissions_msg result;
            const auto o = v.at(trustwave::reg_action_get_registry_permissions_msg::act_name);
            result.id_ = o.at("id").template as<std::string>();
            result.key_ = o.at("key").template as<std::string>();
            return result;
        }
    };
} // namespace tao::json

namespace trustwave {

    class Get_Registry_Permissions_Action final: public Action_Base {
    public:
        Get_Registry_Permissions_Action(): Action_Base(trustwave::reg_action_get_registry_permissions_msg::act_name) { }

        action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::reg_action_get_registry_permissions_msg>>();
        }
    };

} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_ACTIONS_GET_REGISTRY_PERMISSIONS_HPP