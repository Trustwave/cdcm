#ifdef __cplusplus
extern "C" {
#endif
#include "source3/include/includes.h"
#include "rpc_common.h"
#include "../libcli/auth/netlogon_creds_cli.h"
#include "../libcli/auth/libcli_auth.h"
#include "../librpc/gen_ndr/ndr_lsa_c.h"
#include "rpc_client/cli_lsarpc.h"
#include "../librpc/gen_ndr/ndr_netlogon.h"
#include "rpc_client/cli_netlogon.h"
#include "../libcli/smbreadline/smbreadline.h"
#include "../libcli/security/security.h"
#include "passdb.h"
#include "libsmb/libsmb.h"
#include "auth/gensec/gensec.h"
#include "../libcli/smb/smbXcli_base.h"
#include "messages.h"
#include "lib/cmdline_contexts.h"
#include "../librpc/gen_ndr/ndr_samr.h"
#include "rpc_client/cli_pipe.h"
#include "util/data_blob.h"
#include "util_sd.h"


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
#include "rpc_client.hpp"
#include "session.hpp"

#include "credentials.hpp"
using trustwave::rpc_client;
using trustwave::result;
//#include "singleton_runner/authenticated_scan_server.hpp"
//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														rpc_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/3/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
using trustwave::session;

enum pipe_auth_type_spnego {
    PIPE_AUTH_TYPE_SPNEGO_NONE = 0,
    PIPE_AUTH_TYPE_SPNEGO_NTLMSSP,
    PIPE_AUTH_TYPE_SPNEGO_KRB5
};
static bool want_mxac;
static struct security_descriptor *get_secdesc(struct cli_state *cli, const char *filename)
{
    uint16_t fnum = (uint16_t)-1;
    struct security_descriptor *sd;
    NTSTATUS status;
    uint32_t sec_info;
    uint32_t desired_access = 0;
    sec_info = SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL;
    if (sec_info & (SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL)) {
        desired_access |= SEC_STD_READ_CONTROL;
    }
    if (sec_info & SECINFO_SACL) {
        desired_access |= SEC_FLAG_SYSTEM_SECURITY;
    }

    if (desired_access == 0) {
        desired_access |= SEC_STD_READ_CONTROL;
    }

    status = cli_ntcreate(cli, filename, 0, desired_access,
                          0, FILE_SHARE_READ|FILE_SHARE_WRITE,
                          FILE_OPEN, 0x0, 0x0, &fnum, NULL);
    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to open %s: %s\n", filename, nt_errstr(status));
        return NULL;
    }

    status = cli_query_security_descriptor(cli, fnum, sec_info,
                                           talloc_tos(), &sd);

    cli_close(cli, fnum);

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to get security descriptor: %s\n",
               nt_errstr(status));
        return NULL;
    }
    return sd;
}

NTSTATUS cmd_lsa_enum_privilege(struct rpc_pipe_client* cli, TALLOC_CTX* mem_ctx_)
{
    struct policy_handle pol;
    NTSTATUS status, result;
    lsa_PrivArray priv_array;
    dcerpc_binding_handle* b = cli->binding_handle;

    uint32_t enum_context = 0;
    uint32_t pref_max_length = 0x1000;

    status = rpccli_lsa_open_policy(cli, mem_ctx_, True, SEC_FLAG_MAXIMUM_ALLOWED, &pol);

    if(!NT_STATUS_IS_OK(status)) goto done;

    status = dcerpc_lsa_EnumPrivs(b, mem_ctx_, &pol, &enum_context, &priv_array, pref_max_length, &result);
    if(!NT_STATUS_IS_OK(status)) goto done;
    if(!NT_STATUS_IS_OK(result)) {
        status = result;
        goto done;
    }

    /* Print results */
    printf("found %d privileges\n\n", priv_array.count);

    for(uint32_t i = 0; i < priv_array.count; i++) {
        printf("%s \t\t%d:%d (0x%x:0x%x)\n",
               priv_array.privs[i].name.string ? priv_array.privs[i].name.string : "*unknown*",
               priv_array.privs[i].luid.high, priv_array.privs[i].luid.low, priv_array.privs[i].luid.high,
               priv_array.privs[i].luid.low);
    }

    dcerpc_lsa_Close(b, mem_ctx_, &pol, &result);
done:
    return status;
}

rpc_client::rpc_client() { /*mem_ctx_ = talloc_stackframe();*/ }
rpc_client::~rpc_client()
{
    if(cli_ != nullptr) { cli_shutdown(cli_); }
    TALLOC_FREE(mem_ctx_);
}
int rpc_client::connect(const session& sess,const std::string& share,const std::string& device,const ndr_interface_table* table)
{
    creds_ = ::cli_credentials_init(mem_ctx_);
    cli_credentials_set_domain(creds_, sess.creds().domain().c_str(), CRED_SPECIFIED);
    cli_credentials_set_username(creds_, sess.creds().username().c_str(), CRED_SPECIFIED);
    cli_credentials_set_password(creds_, sess.creds().password().c_str(), CRED_SPECIFIED);
    cli_credentials_set_workstation(creds_, sess.creds().workstation().c_str(), CRED_SPECIFIED);

    smb_init_locale();

    /* the following functions are part of the Samba debugging
       facilities.  See lib/debug.c */
    setup_logging("rpcclient", DEBUG_STDOUT);
    lp_set_cmdline("log level", "1");

    if(!init_names()) { return -1; }

    struct dcerpc_binding* binding = nullptr;
    NTSTATUS nt_status = dcerpc_parse_binding(mem_ctx_, sess.remote().c_str(), &binding);

    if(!NT_STATUS_IS_OK(nt_status)) { return -1; }

    nt_status = dcerpc_binding_set_transport(binding, NCACN_NP);
    if(!NT_STATUS_IS_OK(nt_status)) { return -1; }

    nt_status = cli_full_connection_creds(&cli_, nullptr, dcerpc_binding_get_string_option(binding, "host"), nullptr, 0,
                                          share.c_str(), device.c_str(), creds_, 0, SMB_SIGNING_IPC_DEFAULT);

    if(!NT_STATUS_IS_OK(nt_status)) {
        DEBUG(0, ("Cannot connect to server.  Error was %s\n", nt_errstr(nt_status)));
        return -1;
    }

    cli_set_timeout(cli_, 10000);
    auto ntresult = cli_rpc_pipe_open_with_creds(cli_, table, dcerpc_binding_get_transport(binding), DCERPC_AUTH_TYPE_NTLMSSP,
                                                 DCERPC_AUTH_LEVEL_PRIVACY, smbXcli_conn_remote_name(cli_->conn),
                                                 creds_, &pipe_handle_);
    if(!NT_STATUS_IS_OK(ntresult)) {
        printf("Could not initialise %s. Error was %s\n", table->name, nt_errstr(ntresult));
        return -1; // ntresult;
    }
    return 0;
}
cli_state	*rpc_client::cli()
{
    return cli_;
}
rpc_pipe_client	*rpc_client::pipe_handle()
{
    return pipe_handle_;
}