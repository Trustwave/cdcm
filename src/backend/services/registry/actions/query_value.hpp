//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														query_value.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 May 2019
// Comments:

#ifndef TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "registry_action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    struct reg_action_query_value_msg: public action_msg {
        static constexpr std::string_view act_name{"query_value"};
        reg_action_query_value_msg(): action_msg(act_name) {}

        std::string key_;
        std::string value_;
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::reg_action_query_value_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>,
                        TAO_JSON_BIND_REQUIRED("key", &trustwave::reg_action_query_value_msg::key_),
                        TAO_JSON_BIND_REQUIRED("value", &trustwave::reg_action_query_value_msg::value_)> {
        TAO_JSON_DEFAULT_KEY(trustwave::reg_action_query_value_msg::act_name.data());
        template<template<typename...> class Traits>
        static trustwave::reg_action_query_value_msg as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::reg_action_query_value_msg result;
            const auto o = v.at(trustwave::reg_action_query_value_msg::act_name);
            result.id_ = o.at("id").template as<std::string>();
            result.key_ = o.at("key").template as<std::string>();
            result.value_ = o.at("value").template as<std::string>();
            return result;
        }
    };
} // namespace tao::json

namespace trustwave {

    class Query_Value_Action final: public Registry_Action {
    public:
        Query_Value_Action(): Registry_Action(trustwave::reg_action_query_value_msg::act_name) {}

        int act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::reg_action_query_value_msg>>();
        }
    };

} // namespace trustwave

#endif /* TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_ */
