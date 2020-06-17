//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_client2.cpp
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
#include "rpc_client/rpc_client.h"
#include "registry.h"
#include "utils/net_registry_util.h"
#include "../librpc/gen_ndr/ndr_winreg_c.h"
#include "../librpc/gen_ndr/ndr_security.h"
#include "../libcli/security/display_sec.h"
#include "../libcli/registry/util_reg.h"
#include "client.h"

#ifdef __cplusplus
}
#endif
#include "registry_client2.hpp"
#include "../common/security_descriptor_utils.hpp"
#include <unordered_map>
using trustwave::registry_client2;
using trustwave::result;
namespace {
    bool split_hive_key(const std::string& path, std::string& hivename, std::string& subkeyname)
    {
        if(path.empty()) { return false; }
        auto pos = path.find("\\");
        if(std::string::npos != pos) {
            subkeyname.assign(path.substr(pos + 1));
            hivename.assign(path.substr(0, pos));
        }
        else {
            subkeyname.assign(path);
        }

        return true;
    }
    bool reg_hive_key(const std::string& fullname, uint32_t& reg_type, std::string& subkeyname)
    {
        std::string hivename;
        std::string tmp_keyname;
        bool ret = split_hive_key(fullname, hivename, tmp_keyname);
        if(!ret) { return ret; }

        subkeyname = tmp_keyname;
        if(subkeyname.empty()) { return false; }
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
        try {
            reg_type = hives.at(hivename);
        }
        catch(const std::out_of_range& e) {
            // fixme assaf add notif in log
        }
        return true;
    }
    static NTSTATUS dcerpc_winreg_Connect(struct dcerpc_binding_handle* b, TALLOC_CTX* mem_ctx, uint32_t reg_type,
                                          uint32_t access_mask, struct policy_handle* reg_hnd, WERROR* werr)
    {
        ZERO_STRUCTP(reg_hnd);

        switch(reg_type) {
            case HKEY_CLASSES_ROOT:
                return dcerpc_winreg_OpenHKCR(b, mem_ctx, nullptr, access_mask, reg_hnd, werr);

            case HKEY_LOCAL_MACHINE:
                return dcerpc_winreg_OpenHKLM(b, mem_ctx, nullptr, access_mask, reg_hnd, werr);

            case HKEY_USERS:
                return dcerpc_winreg_OpenHKU(b, mem_ctx, nullptr, access_mask, reg_hnd, werr);

            case HKEY_CURRENT_USER:
                return dcerpc_winreg_OpenHKCU(b, mem_ctx, nullptr, access_mask, reg_hnd, werr);

            case HKEY_PERFORMANCE_DATA:
                return dcerpc_winreg_OpenHKPD(b, mem_ctx, nullptr, access_mask, reg_hnd, werr);

            default:
                /* fall through to end of function */
                break;
        }

        return NT_STATUS_INVALID_PARAMETER;
    }

    NTSTATUS registry_getsd(TALLOC_CTX* mem_ctx, struct dcerpc_binding_handle* b, struct policy_handle* key_hnd,
                            uint32_t sec_info, struct KeySecurityData* sd, WERROR* werr)
    {
        return dcerpc_winreg_GetKeySecurity(b, mem_ctx, key_hnd, sec_info, sd, werr);
    }

    NTSTATUS registry_openkey(TALLOC_CTX* mem_ctx, struct rpc_pipe_client* pipe_hnd, const std::string& name,
                              uint32_t access_mask, struct policy_handle* hive_hnd, struct policy_handle* key_hnd)
    {
        uint32_t hive;
        std::string keyname;
        if(!reg_hive_key(name, hive, keyname)) { return NT_STATUS_INVALID_PARAMETER; }
        struct winreg_String key;
        ZERO_STRUCT(key);
        key.name = talloc_strdup(mem_ctx, keyname.c_str());
        WERROR werr;
        struct dcerpc_binding_handle* b = pipe_hnd->binding_handle;
        NTSTATUS status = dcerpc_winreg_Connect(b, mem_ctx, hive, access_mask, hive_hnd, &werr);
        if(!(NT_STATUS_IS_OK(status))) { return status; }
        if(!W_ERROR_IS_OK(werr)) { return werror_to_ntstatus(werr); }

        status = dcerpc_winreg_OpenKey(b, mem_ctx, hive_hnd, key, 0, access_mask, key_hnd, &werr);
        if(!(NT_STATUS_IS_OK(status))) {
            dcerpc_winreg_CloseKey(b, mem_ctx, hive_hnd, &werr);
            return status;
        }
        if(!(W_ERROR_IS_OK(werr))) {
            WERROR _werr;
            dcerpc_winreg_CloseKey(b, mem_ctx, hive_hnd, &_werr);
            return werror_to_ntstatus(werr);
        }
        return NT_STATUS_OK;
    }
}
result trustwave::registry_client2::open_key(const std::string& k)
{
    uint32_t access_mask = SEC_FLAG_MAXIMUM_ALLOWED | SEC_FLAG_SYSTEM_SECURITY;
    NTSTATUS status
        = registry_openkey(talloc_tos(), client_->pipe_handle(), k, access_mask, &ctx_.pol_hive, &ctx_.pol_key);
    if(!NT_STATUS_IS_OK(status)) {
        fprintf(stderr, "registry_openkey failed: %s\n", nt_errstr(status));
        return  {false, ntstatus_to_werror(status)};
    }
    return  {true, ntstatus_to_werror(status)};

}
result trustwave::registry_client2::connect(const session& sess)
{
    return client_->connect(sess, "IPC$", "IPC", &ndr_table_winreg);
}
result trustwave::registry_client2::get_sd(trustwave::sd_utils::Security_Descriptor_str &outsd)
{
    NTSTATUS status;
    dcerpc_binding_handle* b = client_->pipe_handle()->binding_handle;
    static const auto out = [this, &status, &b]() -> result {
        WERROR werr;
        dcerpc_winreg_CloseKey(b, talloc_tos(), &ctx_.pol_key, &werr);
        dcerpc_winreg_CloseKey(b, talloc_tos(), &ctx_.pol_hive, &werr);
        return  {NT_STATUS_IS_OK(status), ntstatus_to_werror(status)};

    };
    struct KeySecurityData* sd = talloc_zero(talloc_tos(), struct KeySecurityData);
    if(!sd) {
        status = NT_STATUS_NO_MEMORY;
        return out();
    }
    sd->size = 0x1000;
    uint32_t sec_info = SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL;
    WERROR werr;
    status = registry_getsd(talloc_tos(), b, &ctx_.pol_key, sec_info, sd, &werr);
    if(!NT_STATUS_IS_OK(status)) {
        fprintf(stderr, "getting sd failed: %s\n", nt_errstr(status));
        return out();
    }
    if(!W_ERROR_IS_OK(werr)) {
        status = werror_to_ntstatus(werr);
        fprintf(stderr, "getting sd failed: %s\n", win_errstr(werr));
        return out();
    }
    DATA_BLOB blob;
    blob.data = sd->data;
    blob.length = sd->size;
    security_descriptor sec_desc;
    ndr_err_code ndr_err
        = ndr_pull_struct_blob(&blob, talloc_tos(), &sec_desc, (ndr_pull_flags_fn_t)ndr_pull_security_descriptor);
    if(!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
        status = ndr_map_error2ntstatus(ndr_err);
        return out();
    }
    status = NT_STATUS_OK;
    display_sec_desc(&sec_desc);
    outsd = sd_utils::get_sd_str(client_->cli(), &sec_desc, sd_utils::entity_type::REGISTRY);
    return out();
}
trustwave::registry_client2::~registry_client2() {  }
