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
bool trustwave::reg_hive_key(const std::string& fullname, uint32_t& reg_type, std::string& subkeyname)
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