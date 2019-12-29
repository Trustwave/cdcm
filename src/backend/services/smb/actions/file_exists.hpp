//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_exists.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/28/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_SERVICES_SMB_FILE_EXISTS_HPP
#define SRC_SERVICES_SMB_FILE_EXISTS_HPP
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
#include <iostream>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    struct smb_file_exists_msg : public single_param_action_msg {
        static constexpr std::string_view act_name{"file_exists"};

        smb_file_exists_msg() :
                single_param_action_msg(act_name) {
        }

        explicit smb_file_exists_msg(const single_param_action_msg &o) :
                single_param_action_msg(o, act_name) {
        }

    };
}
namespace tao::json {
    template<>
    struct traits<trustwave::smb_file_exists_msg> : binding::object<
            binding::inherit < traits<trustwave::single_param_action_msg> > > {
        TAO_JSON_DEFAULT_KEY(trustwave::smb_file_exists_msg::act_name.data());

        template< template< typename... > class Traits >
        static trustwave::smb_file_exists_msg as( const tao::json::basic_value< Traits >& v )
        {
            trustwave::smb_file_exists_msg result;
            const auto o = v.at(trustwave::smb_file_exists_msg::act_name);
            result.param = o.at( "param" ).template as< std::string >();
            return result;
        }
    };
}
namespace trustwave{
    class SMB_File_Exists: public Action_Base
    {
    public:

        SMB_File_Exists() :
                Action_Base(trustwave::smb_file_exists_msg::act_name)
        {
        }

        int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::smb_file_exists_msg>>();
        }

    };

}

#endif //SRC_SERVICES_SMB_FILE_EXISTS_HPP