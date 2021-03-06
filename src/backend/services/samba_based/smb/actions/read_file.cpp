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
#include <unistd.h>
#include <string>
#include <memory>
#include <boost/algorithm/string/replace.hpp>
#include "../smb_client.hpp"
#include "read_file.hpp"

#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
using trustwave::SMB_Read_File;
using action_status = trustwave::Action_Base::action_status;
namespace {
    namespace {
        const char base64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "0123456789+/";
        // A constant defined by Base64 algorithm
        const unsigned char PAD = '=';
    } // namespace

    static constexpr size_t base64_encoded_length(size_t origLen) { return (((origLen + 2) / 3) << 2); }

    std::string base64_encode(const char* inBuf, ssize_t inLen)
    {
        if(nullptr == inBuf || 0 == inLen) {
            AU_LOG_ERROR("inLen : %d inBuf : ", inLen, inBuf);
            return std::string();
        }

        unsigned int bitsContainer = 0; // A container for 24 bits from the input stream
        unsigned char currByte; // A current byte from the input stream
        size_t charCount = 0; // Count byte tripples
        size_t outPos = 0; // Current letter in the output stream

        std::string ret(base64_encoded_length(inLen), '\0');
        while(inLen--) // Scan the input bit stream
        {
            currByte = *(inBuf++);
            bitsContainer |= currByte;
            charCount++;

            if(3 == charCount) {
                ret[outPos++] = base64Alphabet[bitsContainer >> 18 & 0x3f];
                ret[outPos++] = base64Alphabet[(bitsContainer >> 12) & 0x3f];
                ret[outPos++] = base64Alphabet[(bitsContainer >> 6) & 0x3f];
                ret[outPos++] = base64Alphabet[bitsContainer & 0x3f];
                bitsContainer = 0;
                charCount = 0;
            }
            else {
                bitsContainer <<= 8;
            }
        }

        if(charCount) {
            bitsContainer <<= 16 - (8 * charCount);
            ret[outPos++] = base64Alphabet[bitsContainer >> 18 & 0x3f];
            ret[outPos++] = base64Alphabet[(bitsContainer >> 12) & 0x3f];
            if(charCount == 1) { ret[outPos++] = PAD; }
            else {
                ret[outPos++] = base64Alphabet[(bitsContainer >> 6) & 0x3f];
            }
            ret[outPos++] = PAD;
        }
        // AU_LOG_ERROR("ret : %s",ret.c_str());
        return ret;
    }
} // namespace
action_status
SMB_Read_File::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->set_response_for_error(CDCM_ERROR::SESSION_NOT_FOUND);
        return action_status::FAILED;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_read_file_msg>(action);
    if(!smb_action) {
        AU_LOG_ERROR("Failed dynamic cast");
        res->set_response_for_error(CDCM_ERROR::INTERNAL_ERROR);
        return action_status::FAILED;
    }
    if( smb_action->path_.empty())
    {
        res->set_response_for_error(CDCM_ERROR::PATH_IS_MANDATORY);
        return action_status::FAILED;
    }
    if(smb_action->offset_.empty()) { smb_action->offset_ = "0"; }
    if(smb_action->size_.empty()) { smb_action->size_ = "0"; }

    if( std::stoll(smb_action->offset_) < 0 || std::stoll(smb_action->size_)  < 0 )
    {
        res->set_response_for_error(CDCM_ERROR::BAD_PARAMETER);
        return action_status::FAILED;
    }
    std::string path = boost::replace_all_copy(smb_action->path_, "\\", "/");
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(path);
    trustwave::smb_client rc;
    auto connect_result = rc.open_file(base.c_str());
    if(!connect_result.first) {
        AU_LOG_DEBUG("got smb error: %i - %s", connect_result.second, std::strerror(connect_result.second));
        res->set_response_for_error_with_unique_code_or_msg(CDCM_ERROR::GENERAL_ERROR_WITH_ASSET, connect_result.second, std::strerror(connect_result.second));
        return action_status::FAILED;
    }
    auto off = smb_action->offset_.empty() ? 0 : std::stoul(smb_action->offset_);
    auto sz = smb_action->size_.empty() ? 0 : std::stoul(smb_action->size_);
    if(0 == sz) { sz = rc.file_size() - off; }
    AU_LOG_DEBUG("Received offset: %zu size: %zu", off, sz);
    auto buff = std::make_unique<char[]>(sz);
    if(!buff) {
        res->set_response_for_error(CDCM_ERROR::MEMORY_ALLOCATION_FAILED);
        return action_status::FAILED;
    }
    ssize_t r = rc.read(off, sz, buff.get());
    if(-1 == r) {
        res->set_response_for_error(CDCM_ERROR::READ_FILE_FAILED);
        return action_status::FAILED;
    }

    auto c64_str = base64_encode(buff.get(), r);
    res->set_response_for_success(c64_str);
    return action_status::SUCCEEDED;
}
static std::shared_ptr<SMB_Read_File> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<SMB_Read_File>());
}
