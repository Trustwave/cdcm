//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_cdcm_info.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/8/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_FRONTEND_ACTIONS_GET_CDCM_INFO_HPP
#define SRC_FRONTEND_ACTIONS_GET_CDCM_INFO_HPP
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
    struct local_get_cdcm_info_msg: public action_msg {
        static constexpr std::string_view act_name{"get_cdcm_info"};
        local_get_cdcm_info_msg(): action_msg(act_name) {}
        std::string what;
    };
} // namespace trustwave
namespace tao::json {

    template<>
    struct traits<trustwave::local_get_cdcm_info_msg>:
            binding::object<binding::inherit<traits<trustwave::action_msg>>,
            TAO_JSON_BIND_REQUIRED("what", &trustwave::local_get_cdcm_info_msg::what)> {
    TAO_JSON_DEFAULT_KEY(trustwave::local_get_cdcm_info_msg::act_name.data());
    template<template<typename...> class Traits>
    static trustwave::local_get_cdcm_info_msg as(const tao::json::basic_value<Traits>& v)
    {
        trustwave::local_get_cdcm_info_msg result;
        const auto& object = v.at(trustwave::local_get_cdcm_info_msg::act_name);
        result.id_ = object.at("id").template as<std::string>();
        result.what = object.at("what").template as<std::string>();
        return result;
    }
};
} // namespace tao::json

namespace trustwave {

    class Get_CDCM_Info final: public Action_Base {
        static Dispatcher<Action_Base>::Registrator m_registrator;

    public:
        Get_CDCM_Info(): Action_Base(trustwave::local_get_cdcm_info_msg::act_name) {}
        action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<local_get_cdcm_info_msg>>();
        }
    };

} // namespace trustwave
#endif //SRC_FRONTEND_ACTIONS_GET_CDCM_INFO_HPP