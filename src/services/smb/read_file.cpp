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
    namespace {
        const char base64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "0123456789+/";

// Reverse translation Alphabet -> binary value
// It is also constant array but it is too large to initialize it here.
// There are 256 entries to serve each possible ASCII character
// The array is produced by the following code:
// for ( i = 0; i < 256; i++ ) base64Value[i] = -1;
// for ( i = 0; i < 64; i++ ) base64Value[(int)base64Alphabet[i]] = i;

        const int base64Value[] = {
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
                61, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1,
                -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
                43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1};

// A constant defined by Base64 algorithm
        const unsigned char PAD = '=';
    }

    size_t base64_encoded_length(size_t origLen) {
        return (((origLen + 2) / 3) << 2);
    }


    std::string base64_encode(const char *inBuf, ssize_t inLen
                                        ) {
        if (nullptr == inBuf|| 0 == inLen ) {
            return std::string();
        }

        unsigned int bitsContainer = 0;                  // A container for 24 bits from the input stream
        unsigned char currByte; // A current byte from the input stream
        size_t charCount = 0;   // Count byte tripples
        size_t outPos = 0;      // Current letter in the output stream

        std::string ret;
        ret.reserve(base64_encoded_length(inLen));
        while (inLen--) // Scan the input bit stream
        {
            currByte = *(inBuf++);
            bitsContainer |= currByte;
            charCount++;

            if (3 == charCount) {
                ret[outPos++] = base64Alphabet[bitsContainer >> 18 & 0x3f];
                ret[outPos++] = base64Alphabet[(bitsContainer >> 12) & 0x3f];
                ret[outPos++] = base64Alphabet[(bitsContainer >> 6) & 0x3f];
                ret[outPos++] = base64Alphabet[bitsContainer & 0x3f];
                bitsContainer = 0;
                charCount = 0;
            } else { bitsContainer <<= 8; }
        }

        if (charCount) {
            bitsContainer <<= 16 - (8 * charCount);
            ret[outPos++] = base64Alphabet[bitsContainer >> 18 & 0x3f];
            ret[outPos++] = base64Alphabet[(bitsContainer >> 12) & 0x3f];
            if (charCount == 1) { ret[outPos++] = PAD; } else {
                ret[outPos++] = base64Alphabet[(bitsContainer >> 6) & 0x3f];
            }
            ret[outPos++] = PAD;
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
    auto b64_str = base64_encode(buff,r);
    res->res(b64_str);
    return 0;
}

Dispatcher<Action_Base>::Registrator SMB_Read_File::m_registrator(new SMB_Read_File,
                                                                 authenticated_scan_server::instance().public_dispatcher);
