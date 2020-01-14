//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														start_session.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 20 May 2019
// Comments:

#ifndef TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_
#define TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    struct local_start_session_msg: public action_msg {
        static constexpr std::string_view act_name{"start_session"};
        local_start_session_msg(): action_msg(act_name) {}
        std::string remote;
        std::string domain;
        std::string username;
        std::string password;
        std::string workstation;
    };
} // namespace trustwave
namespace tao::json {

    template<>
    struct traits<trustwave::local_start_session_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>,
                        TAO_JSON_BIND_REQUIRED("remote", &trustwave::local_start_session_msg::remote),
                        TAO_JSON_BIND_REQUIRED("domain", &trustwave::local_start_session_msg::domain),
                        TAO_JSON_BIND_REQUIRED("username", &trustwave::local_start_session_msg::username),
                        TAO_JSON_BIND_REQUIRED("password", &trustwave::local_start_session_msg::password),
                        TAO_JSON_BIND_REQUIRED("workstation", &trustwave::local_start_session_msg::workstation)> {
        TAO_JSON_DEFAULT_KEY(trustwave::local_start_session_msg::act_name.data());
        template<template<typename...> class Traits>
        static trustwave::local_start_session_msg as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::local_start_session_msg result;
            const auto& object = v.at(trustwave::local_start_session_msg::act_name);
            result.id_ = object.at("id").template as<std::string>();
            result.remote = object.at("remote").template as<std::string>();
            result.domain = object.at("domain").template as<std::string>();
            result.username = object.at("username").template as<std::string>();
            result.password = object.at("password").template as<std::string>();
            result.workstation = object.at("workstation").template as<std::string>();
            return result;
        }
    };
} // namespace tao::json

namespace trustwave {

    class Start_Session final: public Action_Base {
        static Dispatcher<Action_Base>::Registrator m_registrator;

    public:
        Start_Session(): Action_Base(trustwave::local_start_session_msg::act_name) {}
        int act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<local_start_session_msg>>();
        }
    };

} // namespace trustwave

#endif /* TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_ */
