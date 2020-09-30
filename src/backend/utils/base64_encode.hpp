//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														base64_encode.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 7/29/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_UTILS_BASE64_ENCODE_HPP
#define SRC_BACKEND_UTILS_BASE64_ENCODE_HPP
namespace trustwave{
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
} // trustwave namespace

#endif // SRC_BACKEND_UTILS_BASE64_ENCODE_HPP