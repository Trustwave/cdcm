//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														security_descriptor_utils.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/15/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif
#include "source3/include/includes.h"
#include "../librpc/gen_ndr/ndr_lsa_c.h"
#include "source3/libsmb/proto.h"
#include "source3/include/client.h"
#include "libcli/smb/smbXcli_base.h"
#include "source3/rpc_client/cli_lsarpc.h"
#include "libcli/security/dom_sid.h"
#include "libcli/security/security.h"
#include "source3/rpc_client/cli_pipe.h"
#ifdef __cplusplus
}

#endif
#include "security_descriptor_utils.hpp"
#include <unordered_map>
#include <sstream>
#include <iostream>
using trustwave::sd_utils::entity_type;
using trustwave::sd_utils::string_vec;
namespace {
    constexpr uint32_t Modify           = 0x301BF;
    constexpr uint32_t Read             = 0x20089;
    constexpr uint32_t Write            = 0x116;

    constexpr uint32_t Read_And_Execute = 0x200A9;//Files,Dirs Only
    constexpr uint32_t Full_Control     = 0x1F01FF;//Files,Dirs Only

    constexpr uint32_t Reg_Full_Control     = 0xF003F;//Registry Only
    constexpr uint32_t Reg_Read_And_Execute = 0x20019;//Registry Only
    constexpr uint32_t Write_Key            = 0x20006;//Registry Only


static const std::unordered_map<entity_type, std::map<uint32_t, std::string,std::greater<uint32_t>>> perm_dir{
        {entity_type::NTFS_FILE,
         {{SEC_FILE_READ_DATA, "FILE_READ_DATA"},//1
          {SEC_FILE_WRITE_DATA, "FILE_WRITE_DATA"},//2
          {SEC_FILE_APPEND_DATA, "FILE_APPEND_DATA"},//4
          {SEC_FILE_READ_EA, "FILE_READ_EA"},//8
          {SEC_FILE_WRITE_EA, "FILE_WRITE_EA"},//16
          {SEC_FILE_EXECUTE, "FILE_EXECUTE"},//32
          {SEC_FILE_READ_ATTRIBUTE, "FILE_READ_ATTRIBUTE"},//128
          {SEC_FILE_WRITE_ATTRIBUTE, "FILE_WRITE_ATTRIBUTE"},//256
          {Modify            ,"Modify"},
          {Read_And_Execute  ,"Read And Execute"},
          {Full_Control      ,"Full Control"},
          {Read              ,"Read"},
          {Write             ,"Write"}

         }
          },
        {entity_type::NTFS_DIR,
         {{SEC_DIR_LIST, "DIR_LIST"}, // 1
             {SEC_DIR_ADD_FILE, "DIR_ADD_FILE"}, // 2
             {SEC_DIR_ADD_SUBDIR, "DIR_ADD_SUBDIR"}, // 4
             {SEC_DIR_READ_EA, "DIR_READ_EA"}, // 8
             {SEC_DIR_WRITE_EA, "DIR_WRITE_EA"}, // 16
             {SEC_DIR_TRAVERSE, "DIR_TRAVERSE"}, // 32
             {SEC_DIR_DELETE_CHILD, "SEC_DIR_DELETE_CHILD"}, // 64
             {SEC_DIR_READ_ATTRIBUTE, "DIR_READ_ATTRIBUTE"}, // 128
             {SEC_DIR_WRITE_ATTRIBUTE, "DIR_WRITE_ATTRIBUTE"} ,// 256
             {Modify            ,"Modify"},
             {Read_And_Execute  ,"Read And Execute"},
             {Full_Control      ,"Full Control"},
             {Read              ,"Read"},
             {Write             ,"Write"},
         }},
        {entity_type::REGISTRY,
         {{SEC_REG_QUERY_VALUE, "Query"},//1
          {SEC_REG_SET_VALUE, "Set Value"},//2
          {SEC_REG_CREATE_SUBKEY, "CreateSubKey"},//4
          {SEC_REG_ENUM_SUBKEYS, "EnumSubKey"},//8
          {SEC_REG_NOTIFY, "Notify"},//16
          {SEC_REG_CREATE_LINK, "Create Link"},//32
             {Reg_Read_And_Execute,"Read Key, Execute Key"},
             {Reg_Full_Control,"Full Control"}

         }},
        {entity_type::SHARE, {{SHARE_ALL_ACCESS, "SHARE_ALL_ACCESS"}, {SHARE_READ_ONLY, "SHARE_READ_ONLY"},{SEC_FILE_APPEND_DATA,"Change"}}},
        {entity_type::GENERIC,
         {
          {GENERIC_ALL_ACCESS, "GENERIC_ALL_ACCESS"},
          {GENERIC_EXECUTE_ACCESS, "GENERIC_EXECUTE_ACCESS"},
          {GENERIC_WRITE_ACCESS, "GENERIC_WRITE_ACCESS"},
          {GENERIC_READ_ACCESS, "GENERIC_READ_ACCESS"}}},
    {entity_type::STD,{

{  SEC_STD_DELETE        ,  "Delete"},
{  SEC_STD_READ_CONTROL  ,  "Read Control"},
{  SEC_STD_WRITE_DAC     ,  "Change Permissions"},
{  SEC_STD_WRITE_OWNER   ,  "Take Ownership"},
{  SEC_STD_SYNCHRONIZE   ,  "Synchronize"}
       }}

    };

    static const std::unordered_map<uint32_t, std::string> sec_desc_ctrl_bits = {
        {SEC_DESC_OWNER_DEFAULTED, "OD"},       {SEC_DESC_GROUP_DEFAULTED, "GD"},
        {SEC_DESC_DACL_PRESENT, "DP"},          {SEC_DESC_DACL_DEFAULTED, "DD"},
        {SEC_DESC_SACL_PRESENT, "SP"},          {SEC_DESC_SACL_DEFAULTED, "SD"},
        {SEC_DESC_DACL_TRUSTED, "DT"},          {SEC_DESC_SERVER_SECURITY, "SS"},
        {SEC_DESC_DACL_AUTO_INHERIT_REQ, "DR"}, {SEC_DESC_SACL_AUTO_INHERIT_REQ, "SR"},
        {SEC_DESC_DACL_AUTO_INHERITED, "DI"},   {SEC_DESC_SACL_AUTO_INHERITED, "SI"},
        {SEC_DESC_DACL_PROTECTED, "PD"},        {SEC_DESC_SACL_PROTECTED, "PS"},
        {SEC_DESC_RM_CONTROL_VALID, "RM"},      {SEC_DESC_SELF_RELATIVE, "SR"},
    };
    static const std::unordered_map<uint32_t, std::string> ace_flags = {{SEC_ACE_FLAG_OBJECT_INHERIT, "OI"},
                                                                        {SEC_ACE_FLAG_CONTAINER_INHERIT, "CI"},
                                                                        {SEC_ACE_FLAG_NO_PROPAGATE_INHERIT, "NP"},
                                                                        {SEC_ACE_FLAG_INHERIT_ONLY, "IO"},
                                                                        {SEC_ACE_FLAG_INHERITED_ACE, "I"}

    };
    template<typename NType>
    static std::string NType_to_hex_string(NType v) {
        std::stringstream ss;
        ss << "0x"<<std::hex <<  v;
        return ss.str();
    }

    template<typename Cont,typename NType>
    static void flags_vector(const Cont cont, NType& flags,string_vec& out_vector)
    {
        NType remain = 0;
        for(const auto e: cont) {
            if(flags >= e.first && (flags & e.first)==e.first)
            {
                out_vector.emplace_back(e.second);
                flags &= ~e.first;
            }
        }

        if(remain > 0)
        {
            out_vector.emplace_back(NType_to_hex_string(remain));
        }
    }
    static void print_ace_flags(std::stringstream& ss, uint8_t flags)
    {
        string_vec v;
        flags_vector(ace_flags, flags,v);
        std::string str;
        for(const auto e: v) { str.append(e).append("|"); }
        if(str[str.length() - 1] == '|') {
            str.erase(str.length() - 1, 1);
            ss << "/" << str << "/";
            return;
        }

        ss << "/" << std::hex << flags << "/";
    }
    static void get_ace_flags(uint8_t flags,string_vec& out_vec) { flags_vector(ace_flags, flags,out_vec); }
    static void get_access_mask(uint32_t mask, entity_type et,string_vec& out_vector)
    {
        auto orig_mask=mask;
        uint32_t new_mask= 0;
        static  const std::map<uint32_t,uint32_t> generic_to_file_generic = {{GENERIC_ALL_ACCESS, FILE_GENERIC_ALL},
                                                         {GENERIC_EXECUTE_ACCESS, FILE_GENERIC_EXECUTE},
                                                         {GENERIC_WRITE_ACCESS, FILE_GENERIC_WRITE},
                                                         {GENERIC_READ_ACCESS, FILE_GENERIC_READ}};
        for (const auto v:generic_to_file_generic) {
            if (v.first == (orig_mask & v.first)) {
                new_mask |= v.second; // add the file generic to new mask
            }
        }
        new_mask &=0x00FFFFFF;// remove the generic
        flags_vector(perm_dir.at(et), orig_mask, out_vector);// extract special permissions
        for (const auto v:perm_dir.at(entity_type::STD)) {
            if (v.first == (new_mask & v.first)) {
                out_vector.emplace_back(v.second);
            }
        }

        if(out_vector.empty()) {
            out_vector.emplace_back(NType_to_hex_string(orig_mask));
        }
    }
    static void SidToString(cli_state* cli, fstring str, const dom_sid* sid)
    {
        static constexpr auto tcon_fail
            = [&](cli_state* cli, smbXcli_tcon* o_conn) { cli_state_restore_tcon(cli, o_conn); };
        static constexpr auto fail = [&](cli_state* cli, rpc_pipe_client* p, smbXcli_tcon* o_conn) {
            TALLOC_FREE(p);
            cli_tdis(cli);
            tcon_fail(cli, o_conn);
        };
        sid_to_fstring(str, sid);
        smbXcli_tcon* o_conn = cli_state_save_tcon(cli);
        NTSTATUS status = cli_tree_connect(cli, "IPC$", "?????", nullptr);
        if(!NT_STATUS_IS_OK(status)) { return tcon_fail(cli, o_conn); }
        rpc_pipe_client* p;
        status = cli_rpc_pipe_open_noauth(cli, &ndr_table_lsarpc, &p);
        if(!NT_STATUS_IS_OK(status)) { return fail(cli, p, o_conn); }
        auto x = talloc_tos();
        policy_handle handle;
        status = rpccli_lsa_open_policy(p, x, True, GENERIC_EXECUTE_ACCESS, &handle);
        if(!NT_STATUS_IS_OK(status)) { return fail(cli, p, o_conn); }
        char** domains;
        char** names;
        enum lsa_SidType* types;
        status = rpccli_lsa_lookup_sids(p, x, &handle, 1, sid, &domains, &names, &types);
        if(!NT_STATUS_IS_OK(status)) { return fail(cli, p, o_conn); }

        char* domain = talloc_move(x, &domains[0]);
        char* name = talloc_move(x, &names[0]);
        if(!NT_STATUS_IS_OK(status)) { return fail(cli, p, o_conn); }

        if(*domain) { slprintf(str, sizeof(fstring) - 1, "%s/%s", domain, name); }
        else {
            fstrcpy(str, name);
        }
        return fail(cli, p, o_conn);
    }
    static void print_ace(cli_state* cli, std::stringstream& ss, security_ace* ace, entity_type et)
    {
        fstring sidstr;
        SidToString(cli, sidstr, &ace->trustee);
        ss << sidstr << ":";

        /* Ace type */

        if(ace->type == SEC_ACE_TYPE_ACCESS_ALLOWED) { ss << "ALLOWED"; }
        else if(ace->type == SEC_ACE_TYPE_ACCESS_DENIED) {
            ss << "DENIED";
        }
        else {
            ss << ace->type;
        }

        print_ace_flags(ss, ace->flags);
        string_vec v;
        flags_vector(perm_dir.at(et), ace->access_mask,v);
        std::string str;
        for(const auto e: v) { str.append(e).append("|"); }
        if(str[str.length() - 1] == '|') { str.erase(str.length() - 1, 1); }
        ss << str;
    }
    static void print_acl_ctrl(std::stringstream& ss, uint16_t ctrl)
    {
        ss << "CONTROL:";
        string_vec v;
        flags_vector(sec_desc_ctrl_bits, ctrl,v);
        std::string str;
        for(const auto e: v) { str.append(e).append("|"); }
        if(str[str.length() - 1] == '|') {
            str.erase(str.length() - 1, 1);
            ss << "\n";
            return;
        }
    }
    static void get_acl_ctrl(uint16_t ctrl,string_vec& out_vec)
    {
        flags_vector(sec_desc_ctrl_bits, ctrl,out_vec);
    }
    static trustwave::sd_utils::ACE_str get_acl(cli_state* cli, security_ace* ace, entity_type et)
    {
        fstring sidstr;
        SidToString(cli, sidstr, &ace->trustee);
        trustwave::sd_utils::ACE_str aa;
        aa.SecurityPrincipal = sidstr;
        if(ace->type == SEC_ACE_TYPE_ACCESS_ALLOWED) { aa.AccessControlType = "ALLOWED"; }
        else if(ace->type == SEC_ACE_TYPE_ACCESS_DENIED) {
            aa.AccessControlType = "DENIED";
        }
        else {
            aa.AccessControlType = std::to_string(ace->type);
        }
        get_ace_flags(ace->flags,aa.AccessControlFlags);
        get_access_mask(ace->access_mask,et,aa.FileSystemRights );
        return aa;
    }

} // namespace

std::ostream& trustwave::operator<<(std::ostream& os, const trustwave::sd_utils::ACE_str& acl)
{
    os << "SecurityPrincipal: " << acl.SecurityPrincipal << "\n"
       << "FileSystemRights: ";
    for(auto e: acl.FileSystemRights) { os << e << "|"; }
    os << "\n"
       << "AccessControlType: " << acl.AccessControlType << "\n"
       << "AccessControlFlags: ";
    for(auto e: acl.AccessControlFlags) { os << e << "|"; }
    os << "\n";
    return os;
}
std::ostream& trustwave::operator<<(std::ostream& os, const trustwave::sd_utils::Security_Descriptor_str& sds)
{
    os << "Revision: " << sds.Revision << "\n"
       << "Control: ";
    for(auto e: sds.Control) { os << e << "|"; }
    os << "\n"
       << "Owner: " << sds.Owner << "\n"
       << "Group: " << sds.Group << "\n"
       << "ACLS: "
       << "\n";
    for(const auto& e: sds.ACLS) { os << e; }
    return os;
}
void trustwave::sd_utils::sec_desc_print(cli_state* cli, std::stringstream& ss, security_descriptor* sd, entity_type et)
{
    fstring sidstr;
    uint32_t i;
    ss << "REVISION:" << sd->revision << "\n";
    print_acl_ctrl(ss, sd->type);

    /* Print owner and group sid */

    if(sd->owner_sid) { SidToString(cli, sidstr, sd->owner_sid); }
    else {
        fstrcpy(sidstr, "");
    }
    ss << "OWNER:" << sidstr << "\n";

    if(sd->group_sid) { SidToString(cli, sidstr, sd->group_sid); }
    else {
        fstrcpy(sidstr, "");
    }
    ss << "GROUP:" << sidstr << "\n";

    /* Print aces */
    for(i = 0; sd->dacl && i < sd->dacl->num_aces; i++) {
        security_ace* ace = &sd->dacl->aces[i];
        ss << "ACL_str:";
        print_ace(cli, ss, ace, et);
        ss << "\n";
    }
}

trustwave::sd_utils::Security_Descriptor_str
trustwave::sd_utils::get_sd_str(cli_state* cli, security_descriptor* sd, entity_type et)
{
    trustwave::sd_utils::Security_Descriptor_str r;
    r.Revision = std::to_string(sd->revision);
    get_acl_ctrl(sd->type,r.Control);
    fstring sidstr;
    if(sd->owner_sid) { SidToString(cli, sidstr, sd->owner_sid); }
    else {
        fstrcpy(sidstr, "");
    }
    r.Owner = sidstr;

    if(sd->group_sid) { SidToString(cli, sidstr, sd->group_sid); }
    else {
        fstrcpy(sidstr, "");
    }
    r.Group = sidstr;
    r.ACLS = trustwave::sd_utils::get_acls(cli, sd, et);

    return r;
}
std::vector<trustwave::sd_utils::ACE_str>
trustwave::sd_utils::get_acls(cli_state* cli, security_descriptor* sd, entity_type et)
{
    std::vector<trustwave::sd_utils::ACE_str> v;
    for(uint32_t i = 0; sd->dacl && i < sd->dacl->num_aces; i++) {
        security_ace* ace = &sd->dacl->aces[i];
        v.emplace_back(get_acl(cli, ace, et));
    }
    return v;
}