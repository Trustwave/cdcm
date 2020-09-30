//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_utils.cpp
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
#include "registry_utils.hpp"
#include <boost/algorithm/string.hpp>
#include <numeric>

static constexpr uint32_t    HKEY_CLASSES_ROOT       =      0x80000000                  ;
static constexpr uint32_t    HKEY_CURRENT_USER       =      0x80000001                  ;
static constexpr uint32_t    HKEY_LOCAL_MACHINE      =      0x80000002                  ;
static constexpr uint32_t    HKEY_USERS              =      0x80000003                  ;
static constexpr uint32_t    HKEY_PERFORMANCE_DATA   =      0x80000004                  ;
static constexpr uint32_t    HKEY_CURRENT_CONFIG     =      0x80000005                  ;
static constexpr uint32_t    HKEY_DYN_DATA           =      0x80000006                  ;
static constexpr uint32_t    HKEY_PERFORMANCE_TEXT   =      0x80000050                  ;
static constexpr uint32_t    HKEY_PERFORMANCE_NLSTEXT=      0x80000060                  ;
static constexpr uint32_t    HKEY_FIRST              =      HKEY_CLASSES_ROOT           ;
static constexpr uint32_t    HKEY_LAST               =      HKEY_PERFORMANCE_NLSTEXT    ;

static const std::unordered_map<std::string, uint32_t> hives{
    {"HKLM",                   HKEY_LOCAL_MACHINE},
    {"HKEY_LOCAL_MACHINE",     HKEY_LOCAL_MACHINE},
    {"HKCR",                   HKEY_CLASSES_ROOT},
    {"HKEY_CLASSES_ROOT",      HKEY_CLASSES_ROOT},
    {"HKU",                    HKEY_USERS},
    {"HKEY_USERS",             HKEY_USERS},
    {"HKCU",                   HKEY_CURRENT_USER},
    {"HKEY_CURRENT_USER",      HKEY_CURRENT_USER},
    {"HKPD",                   HKEY_PERFORMANCE_DATA},
    {"HKEY_PERFORMANCE_DATA",  HKEY_PERFORMANCE_DATA},
};
static const std::unordered_map<uint32_t ,std::string> hive_long_names{
    {  HKEY_LOCAL_MACHINE     ,  "HKEY_LOCAL_MACHINE"   },
    {  HKEY_CLASSES_ROOT      ,  "HKEY_CLASSES_ROOT"    },
    {  HKEY_USERS             ,  "HKEY_USERS"           },
    {  HKEY_CURRENT_USER      ,  "HKEY_CURRENT_USER"    },
    {  HKEY_PERFORMANCE_DATA  ,  "HKEY_PERFORMANCE_DATA"},

};
std::string_view trustwave::hive_long_name(const uint32_t hive)
{
    return hive_long_names.at(hive);
}
void trustwave::split_hive_key(const std::string& path, std::string& hivename, std::string& subkeyname)
{
    static constexpr std::string_view slash("\\");
    std::vector<std::string> strs;
    boost::split(strs,path,boost::is_any_of(slash));
    strs.erase(std::remove_if(strs.begin(),
                              strs.end(),
                                    [&](const std::string s)-> bool
                                    { return s.empty(); }),
               strs.end());
    auto slash_fold = [](std::string a, std::string b) {
        return std::move(a) + slash[0] + b;
    };
    std::string new_path = std::accumulate(std::next(strs.begin()), strs.end(),
                                    strs[0], // start with first element
                                    slash_fold);

    auto pos = new_path.find(slash);
    if(std::string::npos != pos) {
        subkeyname.assign(new_path.substr(pos + 1));
        hivename.assign(new_path.substr(0, pos));
    }
    else {
        subkeyname.assign(new_path);
    }
}
bool trustwave::reg_hive_key(const std::string& fullname, uint32_t & reg_type, std::string& subkeyname)
{
    if(fullname.empty()) { return false; }
    std::string hivename;
    std::string tmp_keyname;
    split_hive_key(fullname, hivename, tmp_keyname);
    auto it = hives.find(hivename);
    if(it == hives.end())
    {
        return false;
    }
    else
    {
        reg_type = it->second;
        subkeyname = tmp_keyname;
    }
    return true;
}