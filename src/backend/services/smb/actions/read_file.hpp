//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														read_file.hpp
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
#ifndef SRC_SERVICES_SMB_READ_FILE_HPP
#define SRC_SERVICES_SMB_READ_FILE_HPP
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
    struct smb_read_file_msg : public action_msg {
        static constexpr std::string_view act_name{"read_file"};
        smb_read_file_msg() :
                action_msg(act_name) {
        }

        std::string path_;
        std::string size_;
        std::string offset_;
    };
}
namespace tao::json {
    template<>
    struct traits<trustwave::smb_read_file_msg> :
            binding::object<binding::inherit<traits<trustwave::action_msg> >,
            TAO_JSON_BIND_REQUIRED("path",&trustwave::smb_read_file_msg::path_),
    TAO_JSON_BIND_OPTIONAL("size",&trustwave::smb_read_file_msg::size_),
    TAO_JSON_BIND_OPTIONAL("offset", &trustwave::smb_read_file_msg::offset_)> {
    TAO_JSON_DEFAULT_KEY(trustwave::smb_read_file_msg::act_name.data());
    template< template< typename... > class Traits >
    static trustwave::smb_read_file_msg as( const tao::json::basic_value< Traits >& v )
    {
        trustwave::smb_read_file_msg result;
        const auto o = v.at(trustwave::smb_read_file_msg::act_name);
        result.path_ = o.at( "path" ).template as< std::string >();
        result.size_ = o.at( "size" ).template as< std::string >();
        result.offset_ =o.at( "offset" ).template as< std::string >();

        return result;
    }

};
}
namespace trustwave {

    class SMB_Read_File: public Action_Base
    {
    public:
        SMB_Read_File() :
                Action_Base( trustwave::smb_read_file_msg::act_name)
        {
        }

        int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::smb_read_file_msg>>();
        }
    };

}
#endif //SRC_SERVICES_SMB_READ_FILE_HPP