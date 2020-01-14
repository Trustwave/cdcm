//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														list_dir.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/4/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_SERVICES_SMB_LIST_DIR_HPP
#define SRC_SERVICES_SMB_LIST_DIR_HPP
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
    struct smb_list_dir_msg: public single_param_action_msg {
        static constexpr std::string_view act_name{"list_dir"};
        smb_list_dir_msg(): single_param_action_msg(act_name) {}

        explicit smb_list_dir_msg(const single_param_action_msg& o): single_param_action_msg(o, act_name) {}
        std::string pattern;
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::smb_list_dir_msg>:
        binding::object<binding::inherit<traits<trustwave::single_param_action_msg>>> {
        TAO_JSON_DEFAULT_KEY(trustwave::smb_list_dir_msg::act_name.data());

        template<template<typename...> class Traits>
        static trustwave::smb_list_dir_msg as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::smb_list_dir_msg result;
            const auto o = v.at(trustwave::smb_list_dir_msg::act_name);
            result.id_ = o.at("id").template as<std::string>();
            result.param = o.at("param").template as<std::string>();
            result.pattern = o.at("pattern").template as<std::string>();
            return result;
        }
    };
} // namespace tao::json
namespace trustwave {

    class SMB_List_Dir final: public Action_Base {
    public:
        SMB_List_Dir(): Action_Base(trustwave::smb_list_dir_msg::act_name) {}

        int act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::smb_list_dir_msg>>();
        }
    };

} // namespace trustwave
#endif // SRC_SERVICES_SMB_LIST_DIR_HPP