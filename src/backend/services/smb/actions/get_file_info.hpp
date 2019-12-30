//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_file_info.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/27/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_SERVICES_SMB_GET_FILE_INFO_HPP
#define SRC_SERVICES_SMB_GET_FILE_INFO_HPP
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave{
    struct smb_get_file_info_msg : public single_param_action_msg {
        static constexpr std::string_view act_name{"get_file_info"};
        smb_get_file_info_msg() :
                single_param_action_msg(act_name) {
        }

        explicit smb_get_file_info_msg(const single_param_action_msg &o) :
                single_param_action_msg(o, act_name) {
        }
    };
}
namespace tao::json {

    template<>
    struct traits<trustwave::smb_get_file_info_msg> : binding::object<
            binding::inherit<traits<trustwave::single_param_action_msg> > > {
        TAO_JSON_DEFAULT_KEY(trustwave::smb_get_file_info_msg::act_name.data());
        template< template< typename... > class Traits >
        static trustwave::smb_get_file_info_msg as( const tao::json::basic_value< Traits >& v )
        {
            std::cerr<<"A!1";
            trustwave::smb_get_file_info_msg result;
            const auto o = v.at(trustwave::smb_get_file_info_msg::act_name);
            result.param = o.at( "param" ).template as< std::string >();
            return result;
        }
    };
}

namespace trustwave {

    class SMB_Get_File_Info: public Action_Base
    {

    public:
        SMB_Get_File_Info() :
                Action_Base(trustwave::smb_get_file_info_msg::act_name)
        {
        }

        int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::smb_get_file_info_msg>>();
        }
    };

}
#endif //SRC_SERVICES_SMB_GET_FILE_INFO_HPP