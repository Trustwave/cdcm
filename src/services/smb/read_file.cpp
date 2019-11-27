//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														read_file.cpp
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
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <unistd.h>
#include <string>

#include "../../clients/smb/smb_client.hpp"
#include "read_file.hpp"

#include "../../common/protocol/msg_types.hpp"
#include "../../common/session.hpp"
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
using namespace trustwave;
namespace {
     const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";


     inline bool is_base64(unsigned char c) {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
                ret += '=';

        }

        return ret;

    }
}
int SMB_Read_File::act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{

    if (!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_read_file_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->path_);
    std::string tmp_name(authenticated_scan_server::instance().settings.downloaded_files_path_+"/" + sess->idstr() + "-" + action->id());
    trustwave::smb_client rc;
    rc.connect(base.c_str());
    auto off = smb_action->offset_.empty()?0:std::stoul(smb_action->offset_);
    auto sz = smb_action->size_.empty()?0:std::stoul(smb_action->size_);
    auto buff = new char[sz];
    rc.read(off,sz,buff);//fixme assaf me return ssize_t
    ssize_t r=100;
auto b64_str = base64_encode(reinterpret_cast<unsigned char *>(buff),r);
    res->res(b64_str);
    return 0;
}

Dispatcher<Action_Base>::Registrator SMB_Read_File::m_registrator(new SMB_Read_File,
                                                                 authenticated_scan_server::instance().public_dispatcher);
