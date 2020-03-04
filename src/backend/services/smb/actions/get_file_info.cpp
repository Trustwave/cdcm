//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_file_info.cpp
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
#include "get_file_info.hpp"
#include <string>
#include <unordered_set>
#include <codecvt>
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
#include "../smb_client.hpp"
#include <regex>
#include "protocol/msg_types.hpp"
#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "pe_context.hpp"
using namespace trustwave;
auto push_back = [](tao::json::events::to_value& c, const std::string& k, const std::string& v) {
    c.begin_object();
    c.key(k);
    c.string(v);
    c.member();
    c.end_object();
    c.element();
};
int SMB_Get_File_Info::act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg> action,
                           std::shared_ptr<result_msg> res)
{
    if(!sess || (sess && sess->id().is_nil())) {
        res->res("Error: Session not found");
        return -1;
    }

    auto smb_action = std::dynamic_pointer_cast<smb_get_file_info_msg>(action);
    std::string base("smb://");
    base.append(sess->remote()).append("/").append(smb_action->param);
    trustwave::smb_client rc;
    auto connect_res = rc.open_file(base.c_str());
    if(!connect_res.first) {
        res->res(std::string("Error: ") + std::string((std::strerror(connect_res.second))));
        return 0;
    }

    pe_context pc(rc);
    if(0 != pc.parse()) {
        res->res("Error: parse file failed");
        return -1;
    }
    std::map<std::u16string, std::u16string> ret;
    static const std::unordered_set<std::u16string> s
        = {u"CompanyName", u"FileDescription", u"FileVersion", u"ProductName", u"ProductVersion"};
    pc.extract_info(ret, s);
    tao::json::events::to_value c;
    c.begin_object();
    c.key("size");
    c.string(std::to_string(rc.file_size()));
    c.member();
    c.key("path");
    c.string("NULL");
    c.member();
    c.key("last_modified");
    c.string(std::to_string(rc.last_modified()));
    c.member();

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    for(const auto& e: ret) {
        auto key = convert.to_bytes(std::u16string(e.first));
        auto val = convert.to_bytes(std::u16string(e.second));

        if(key == "FileVersion") // fix for CDCM-66
        {
            static const std::regex re("([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)\\s+.*");
            std::smatch what;
            if(std::regex_match(val, what, re)) {
                val = what.str(1);
            }
        }
        c.key(key);
        c.string(val.empty() ? std::string("NULL") : val);
        c.member();
    }
    c.end_object();

    res->res(c.value);
    return 0;
}
static std::shared_ptr<SMB_Get_File_Info> instance = nullptr;

// extern function, that declared in "action.hpp", for export the plugin from dll
std::shared_ptr<trustwave::Action_Base> import_action()
{
    return instance ? instance : (instance = std::make_shared<SMB_Get_File_Info>());
}