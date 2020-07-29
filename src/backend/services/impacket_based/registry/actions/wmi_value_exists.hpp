//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_value_exists.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 7/29/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_REGISTRY_ACTIONS_WMI_VALUE_EXISTS_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_REGISTRY_ACTIONS_WMI_VALUE_EXISTS_HPP
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
#include "action.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {
    struct wmi_reg_action_value_exists_msg: public action_msg {
        static constexpr std::string_view act_name{"value_exists"};
        wmi_reg_action_value_exists_msg(): action_msg(act_name) { }

        std::string key_;
        std::string value_;
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::wmi_reg_action_value_exists_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>,
        TAO_JSON_BIND_REQUIRED("key", &trustwave::wmi_reg_action_value_exists_msg::key_),
    TAO_JSON_BIND_REQUIRED("value", &trustwave::wmi_reg_action_value_exists_msg::value_)> {
    TAO_JSON_DEFAULT_KEY(trustwave::wmi_reg_action_value_exists_msg::act_name.data());

    template<template<typename...> class Traits>
    static trustwave::wmi_reg_action_value_exists_msg as(const tao::json::basic_value<Traits>& v)
    {
        trustwave::wmi_reg_action_value_exists_msg result;
        const auto o = v.at(trustwave::wmi_reg_action_value_exists_msg::act_name);
        result.id_ = o.at("id").template as<std::string>();
        result.key_ = o.at("key").template as<std::string>();
        result.value_ = o.at("value").template as<std::string>();
        return result;
    }
};
} // namespace tao::json
namespace trustwave {

    class WMI_Value_Exists_Action final: public Action_Base {
    public:
        WMI_Value_Exists_Action(): Action_Base(trustwave::wmi_reg_action_value_exists_msg::act_name) { }

        action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::wmi_reg_action_value_exists_msg>>();
        }
    };
} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_REGISTRY_ACTIONS_WMI_VALUE_EXISTS_HPP