//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														enumerate_shares.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/22/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_ACTIONS_ENUMERATE_SHARES_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_ACTIONS_ENUMERATE_SHARES_HPP

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
    struct srvsvc_action_enumerate_shares_msg: public action_msg {
        static constexpr std::string_view act_name{"enumerate_shares"};
        srvsvc_action_enumerate_shares_msg(): action_msg(act_name) {}
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::srvsvc_action_enumerate_shares_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>> {
    TAO_JSON_DEFAULT_KEY(trustwave::srvsvc_action_enumerate_shares_msg::act_name.data());
    template<template<typename...> class Traits>
    static trustwave::srvsvc_action_enumerate_shares_msg as(const tao::json::basic_value<Traits>& v)
    {
        trustwave::srvsvc_action_enumerate_shares_msg result;
        const auto o = v.at(trustwave::srvsvc_action_enumerate_shares_msg::act_name);
        result.id_ = o.at("id").template as<std::string>();
        return result;
    }
};
} // namespace tao::json

namespace trustwave {

    class Enumerate_Shares_Action final: public Action_Base {
    public:
        Enumerate_Shares_Action(): Action_Base(trustwave::srvsvc_action_enumerate_shares_msg::act_name) {}

        action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::srvsvc_action_enumerate_shares_msg>>();
        }
    };

} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_ACTIONS_ENUMERATE_SHARES_HPP