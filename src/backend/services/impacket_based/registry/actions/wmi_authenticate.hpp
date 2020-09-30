//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_authenticate.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/31/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_WMI_AUTHENTICATE_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_WMI_AUTHENTICATE_HPP
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
#include "action.hpp"
#include <boost/shared_ptr.hpp>
#include <iostream>
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {
    struct wmi_authenticate_action_msg: public action_msg {
        static constexpr std::string_view act_name{"wmi_authenticate"};
        wmi_authenticate_action_msg(): action_msg(act_name) { }

    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::wmi_authenticate_action_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>> {
    TAO_JSON_DEFAULT_KEY(trustwave::wmi_authenticate_action_msg::act_name.data());

    template<template<typename...> class Traits>
    static trustwave::wmi_authenticate_action_msg as(const tao::json::basic_value<Traits>& v)
    {
        trustwave::wmi_authenticate_action_msg result;
        const auto o = v.at(trustwave::wmi_authenticate_action_msg::act_name);
        result.id_ = o.at("id").template as<std::string>();
        return result;
    }
};
} // namespace tao::json
namespace trustwave {

    class WMI_Authenticate_Action final: public Action_Base {
    public:
        WMI_Authenticate_Action(): Action_Base(trustwave::wmi_authenticate_action_msg::act_name) { }

        action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::wmi_authenticate_action_msg>>();
        }
    };

} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_WMI_AUTHENTICATE_HPP