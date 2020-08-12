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
#ifdef __cplusplus
extern "C" {
#endif
#include "includes.h"
#include "registry.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#include <string>
#include <unordered_map>
namespace trustwave{
    static const std::unordered_map<std::string, uint32_t> hives{
        {"HKLM", HKEY_LOCAL_MACHINE},
        {"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE},
        {"HKCR", HKEY_CLASSES_ROOT},
        {"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT},
        {"HKU", HKEY_USERS},
        {"HKEY_USERS", HKEY_USERS},
        {"HKCU", HKEY_CURRENT_USER},
        {"HKEY_CURRENT_USER", HKEY_CURRENT_USER},
        {"HKPD", HKEY_PERFORMANCE_DATA},
        {"HKEY_PERFORMANCE_DATA", HKEY_PERFORMANCE_DATA},
    };
    static const std::unordered_map<uint32_t,std::string> hive_long_names{
        {  HKEY_LOCAL_MACHINE     ,  "HKEY_LOCAL_MACHINE"   },
        {  HKEY_CLASSES_ROOT      ,  "HKEY_CLASSES_ROOT"    },
        {  HKEY_USERS             ,  "HKEY_USERS"           },
        {  HKEY_CURRENT_USER      ,  "HKEY_CURRENT_USER"    },
        {  HKEY_PERFORMANCE_DATA  ,  "HKEY_PERFORMANCE_DATA"},

    };
    bool split_hive_key(const std::string& path, std::string& hivename, std::string& subkeyname);
    bool reg_hive_key(const std::string& fullname, uint32_t& reg_type, std::string& subkeyname);
}//namespace trustwave
#endif // SRC_BACKEND_UTILS_REGISTRY_UTILS_HPP