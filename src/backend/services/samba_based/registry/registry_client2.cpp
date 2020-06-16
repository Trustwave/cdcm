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
//#include "rpc_common.h"
//#include "../libcli/auth/netlogon_creds_cli.h"
//#include "../libcli/auth/libcli_auth.h"
//#include "../librpc/gen_ndr/ndr_lsa_c.h"
//#include "rpc_client/cli_lsarpc.h"
//#include "../librpc/gen_ndr/ndr_netlogon.h"
//#include "rpc_client/cli_netlogon.h"
//#include "../libcli/smbreadline/smbreadline.h"
//#include "../libcli/security/security.h"
//#include "passdb.h"
//#include "libsmb/libsmb.h"
//#include "auth/gensec/gensec.h"
//#include "../libcli/smb/smbXcli_base.h"
//#include "messages.h"
//#include "lib/cmdline_contexts.h"
//#include "../librpc/gen_ndr/ndr_samr.h"
//#include "rpc_client/cli_pipe.h"
//#include "util/data_blob.h"
//#include "util_sd.h"


#include "rpc_client/rpc_client.h"
#include "registry.h"
#include "utils/net_registry_util.h"
//#include "registry/regfio.h"
#include "../librpc/gen_ndr/ndr_winreg_c.h"
#include "../librpc/gen_ndr/ndr_security.h"
#include <assert.h>
#include "../libcli/security/display_sec.h"
#include "../libcli/registry/util_reg.h"
#include "client.h"

#ifdef __cplusplus
}
#endif
#include "registry_client2.hpp"
#include "../common/security_descriptor_utils.hpp"
#include <sstream>
WERROR split_hive_key(TALLOC_CTX *ctx, const char *path, char **hivename,
                      char **subkeyname)
{
    char *p;
    const char *tmp_subkeyname;

    if ((path == NULL) || (hivename == NULL) || (subkeyname == NULL)) {
        return WERR_INVALID_PARAMETER;
    }

    if (strlen(path) == 0) {
        return WERR_INVALID_PARAMETER;
    }

    if (strchr(path, '\\') == NULL) {
        *hivename = talloc_string_sub(ctx, path, "/", "\\");
    } else {
        *hivename = talloc_strdup(ctx, path);
    }

    if (*hivename == NULL) {
        return WERR_NOT_ENOUGH_MEMORY;
    }

    /* strip trailing '\\' chars */
    p = strrchr(*hivename, '\\');
    while ((p != NULL) && (p[1] == '\0')) {
        *p = '\0';
        p = strrchr(*hivename, '\\');
    }

    p = strchr(*hivename, '\\');

    if ((p == NULL) || (*p == '\0')) {
        /* just the hive - no subkey given */
        tmp_subkeyname = "";
    } else {
        *p = '\0';
        tmp_subkeyname = p+1;
    }
    *subkeyname = talloc_strdup(ctx, tmp_subkeyname);
    if (*subkeyname == NULL) {
        return WERR_NOT_ENOUGH_MEMORY;
    }

    return WERR_OK;
}
bool reg_hive_key(TALLOC_CTX *ctx, const char *fullname,
                  uint32_t *reg_type, const char **key_name)
{
    WERROR werr;
    char *hivename = NULL;
    char *tmp_keyname = NULL;
    bool ret = false;
    TALLOC_CTX *tmp_ctx = talloc_stackframe();

    werr = split_hive_key(tmp_ctx, fullname, &hivename, &tmp_keyname);
    if (!W_ERROR_IS_OK(werr)) {
        goto done;
    }

    *key_name = talloc_strdup(ctx, tmp_keyname);
    if (*key_name == NULL) {
        goto done;
    }

    if (strequal(hivename, "HKLM") ||
        strequal(hivename, "HKEY_LOCAL_MACHINE"))
    {
        (*reg_type) = HKEY_LOCAL_MACHINE;
    } else if (strequal(hivename, "HKCR") ||
               strequal(hivename, "HKEY_CLASSES_ROOT"))
    {
        (*reg_type) = HKEY_CLASSES_ROOT;
    } else if (strequal(hivename, "HKU") ||
               strequal(hivename, "HKEY_USERS"))
    {
        (*reg_type) = HKEY_USERS;
    } else if (strequal(hivename, "HKCU") ||
               strequal(hivename, "HKEY_CURRENT_USER"))
    {
        (*reg_type) = HKEY_CURRENT_USER;
    } else if (strequal(hivename, "HKPD") ||
               strequal(hivename, "HKEY_PERFORMANCE_DATA"))
    {
        (*reg_type) = HKEY_PERFORMANCE_DATA;
    } else {
        DEBUG(10,("reg_hive_key: unrecognised hive key %s\n",
            fullname));
        goto done;
    }

    ret = true;

    done:
    TALLOC_FREE(tmp_ctx);
    return ret;
}
static NTSTATUS dcerpc_winreg_Connect(struct dcerpc_binding_handle *b, TALLOC_CTX *mem_ctx,
                                      uint32_t reg_type, uint32_t access_mask,
                                      struct policy_handle *reg_hnd, WERROR *werr)
{
    ZERO_STRUCTP(reg_hnd);

    switch (reg_type)
    {
        case HKEY_CLASSES_ROOT:
            return dcerpc_winreg_OpenHKCR(b, mem_ctx, NULL,
                                          access_mask, reg_hnd, werr);

        case HKEY_LOCAL_MACHINE:
            return dcerpc_winreg_OpenHKLM(b, mem_ctx, NULL,
                                          access_mask, reg_hnd, werr);

        case HKEY_USERS:
            return dcerpc_winreg_OpenHKU(b, mem_ctx, NULL,
                                         access_mask, reg_hnd, werr);

        case HKEY_CURRENT_USER:
            return dcerpc_winreg_OpenHKCU(b, mem_ctx, NULL,
                                          access_mask, reg_hnd, werr);

        case HKEY_PERFORMANCE_DATA:
            return dcerpc_winreg_OpenHKPD(b, mem_ctx, NULL,
                                          access_mask, reg_hnd, werr);

        default:
            /* fall through to end of function */
            break;
    }

    return NT_STATUS_INVALID_PARAMETER;
}

NTSTATUS registry_getsd(TALLOC_CTX *mem_ctx,
                        struct dcerpc_binding_handle *b,
                        struct policy_handle *key_hnd,
                        uint32_t sec_info,
                        struct KeySecurityData *sd,
                        WERROR *werr)
{
    return dcerpc_winreg_GetKeySecurity(b, mem_ctx, key_hnd,
                                        sec_info, sd, werr);
}
//static NTSTATUS rpc_registry_getsd_internal(
//    struct cli_state *cli,
//    struct rpc_pipe_client *pipe_hnd,
//    TALLOC_CTX *mem_ctx,const std::string& k)
//{
//    struct policy_handle pol_hive, pol_key;
//    NTSTATUS status;
//    WERROR werr;
//    enum ndr_err_code ndr_err;
//    struct KeySecurityData *sd = NULL;
//    uint32_t sec_info;
//    DATA_BLOB blob;
//    struct security_descriptor sec_desc;
//    uint32_t access_mask = SEC_FLAG_MAXIMUM_ALLOWED |
//                           SEC_FLAG_SYSTEM_SECURITY;
//    struct dcerpc_binding_handle *b = pipe_hnd->binding_handle;
//
//
//
//    status = registry_openkey(mem_ctx, pipe_hnd,k.c_str(),
//                              access_mask,
//                              &pol_hive, &pol_key);
//    if (!NT_STATUS_IS_OK(status)) {
//        fprintf(stderr, "registry_openkey failed: %s\n",
//                nt_errstr(status));
//        return status;
//    }
//
//    sd = talloc_zero(mem_ctx, struct KeySecurityData);
//    if (!sd) {
//        status = NT_STATUS_NO_MEMORY;
//        goto out;
//    }
//
//    sd->size = 0x1000;
//
//
//    sec_info = SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL;
//
//    status = registry_getsd(mem_ctx, b, &pol_key, sec_info, sd, &werr);
//    if (!NT_STATUS_IS_OK(status)) {
//        fprintf(stderr, "getting sd failed: %s\n",
//                nt_errstr(status));
//        goto out;
//    }
//    if (!W_ERROR_IS_OK(werr)) {
//        status = werror_to_ntstatus(werr);
//        fprintf(stderr, "getting sd failed: %s\n",
//                win_errstr(werr));
//        goto out;
//    }
//
//    blob.data = sd->data;
//    blob.length = sd->size;
//
//    ndr_err = ndr_pull_struct_blob(&blob, mem_ctx, &sec_desc,
//                                   (ndr_pull_flags_fn_t)ndr_pull_security_descriptor);
//    if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
//        status = ndr_map_error2ntstatus(ndr_err);
//        goto out;
//    }
//    status = NT_STATUS_OK;
//
//    display_sec_desc(&sec_desc);
//
//    out:
//    dcerpc_winreg_CloseKey(b, mem_ctx, &pol_key, &werr);
//    dcerpc_winreg_CloseKey(b, mem_ctx, &pol_hive, &werr);
//
//    return status;
//}
NTSTATUS registry_openkey(TALLOC_CTX *mem_ctx,
                          struct rpc_pipe_client *pipe_hnd,
                          const char *name, uint32_t access_mask,
                          struct policy_handle *hive_hnd,
                          struct policy_handle *key_hnd)
{
    uint32_t hive;
    NTSTATUS status;
    WERROR werr;
    struct winreg_String key;
    struct dcerpc_binding_handle *b = pipe_hnd->binding_handle;

    ZERO_STRUCT(key);

    if (!reg_hive_key(mem_ctx, name, &hive, &key.name)) {
        return NT_STATUS_INVALID_PARAMETER;
    }

    status = dcerpc_winreg_Connect(b, mem_ctx, hive, access_mask,
                                   hive_hnd, &werr);
    if (!(NT_STATUS_IS_OK(status))) {
        return status;
    }
    if (!W_ERROR_IS_OK(werr)) {
        return werror_to_ntstatus(werr);
    }

    status = dcerpc_winreg_OpenKey(b, mem_ctx, hive_hnd, key, 0,
                                   access_mask, key_hnd, &werr);
    if (!(NT_STATUS_IS_OK(status))) {
        dcerpc_winreg_CloseKey(b, mem_ctx, hive_hnd, &werr);
        return status;
    }
    if (!(W_ERROR_IS_OK(werr))) {
        WERROR _werr;
        dcerpc_winreg_CloseKey(b, mem_ctx, hive_hnd, &_werr);
        return werror_to_ntstatus(werr);
    }

    return NT_STATUS_OK;
}
NTSTATUS trustwave::registry_client2::open_key(const std::string& k)
{
    NTSTATUS status;
//            WERROR werr;
//            enum ndr_err_code ndr_err;
//            struct KeySecurityData *sd = NULL;
    uint32_t sec_info;
//            DATA_BLOB blob;
//            struct security_descriptor sec_desc;
    uint32_t access_mask = SEC_FLAG_MAXIMUM_ALLOWED |
                           SEC_FLAG_SYSTEM_SECURITY;
//            struct dcerpc_binding_handle *b = client_->pipe_handle()->binding_handle;



    status = registry_openkey(mem_ctx_, client_->pipe_handle(),k.c_str(),
                              access_mask,
                              &ctx_.pol_hive, &ctx_.pol_key);
    if (!NT_STATUS_IS_OK(status)) {
        fprintf(stderr, "registry_openkey failed: %s\n",
                nt_errstr(status));
        return status;
    }
    return status;
}
int trustwave::registry_client2::connect(const session& sess)
{
    return client_->connect(sess, "IPC$","IPC", &ndr_table_winreg);
}
NTSTATUS trustwave::registry_client2::get_sd()
{
    NTSTATUS status;
    dcerpc_binding_handle *b = client_->pipe_handle()->binding_handle;
    static const auto out = [this,&status,&b]()->NTSTATUS{
        WERROR werr;
        dcerpc_winreg_CloseKey(b, mem_ctx_, &ctx_.pol_key, &werr);
        dcerpc_winreg_CloseKey(b, mem_ctx_, &ctx_.pol_hive, &werr);
        return status;
    };
    struct KeySecurityData *sd = talloc_zero(mem_ctx_, struct KeySecurityData);
    if (!sd) {
        status = NT_STATUS_NO_MEMORY;
        return out();
    }
    sd->size = 0x1000;
    uint32_t sec_info = SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL;
    WERROR werr;
    status = registry_getsd(mem_ctx_, b, &ctx_.pol_key, sec_info, sd, &werr);
    if (!NT_STATUS_IS_OK(status)) {
        fprintf(stderr, "getting sd failed: %s\n",
                nt_errstr(status));
        return out();

    }
    if (!W_ERROR_IS_OK(werr)) {
        status = werror_to_ntstatus(werr);
        fprintf(stderr, "getting sd failed: %s\n",
                win_errstr(werr));
        return out();

    }
    DATA_BLOB blob;
    blob.data = sd->data;
    blob.length = sd->size;
    security_descriptor sec_desc;
    ndr_err_code ndr_err = ndr_pull_struct_blob(&blob, mem_ctx_, &sec_desc,
                                   (ndr_pull_flags_fn_t)ndr_pull_security_descriptor);
    if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
        status = ndr_map_error2ntstatus(ndr_err);
        return out();

    }
    status = NT_STATUS_OK;
    display_sec_desc(&sec_desc);
    sd_utils::get_sd_str(client_->cli(),&sec_desc,sd_utils::entity_type::REGISTRY);
    //std::stringstream ss;
   // sd_utils::sec_desc_print(client_->cli(),ss,&sec_desc);
    //    std::cerr<<ss.str();
    return out();

}
trustwave::registry_client2::~registry_client2()
{ talloc_free(mem_ctx_); }
