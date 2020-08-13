//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_utils.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/12/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_UTILS_REGISTRY_UTILS_HPP
#define SRC_BACKEND_UTILS_REGISTRY_UTILS_HPP

#include <string>
#include <unordered_map>

namespace trustwave{
    std::string_view hive_long_name(const uint32_t);
    void split_hive_key(const std::string& path, std::string& hivename, std::string& subkeyname);
    bool reg_hive_key(const std::string& fullname, uint32_t & reg_type, std::string& subkeyname);
}//namespace trustwave
#endif // SRC_BACKEND_UTILS_REGISTRY_UTILS_HPP