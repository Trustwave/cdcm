//===========================================================================
// Trustwave ltd. @{SRCH}
//								test.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 1 Jul 2019
// Comments: 

#ifdef __cplusplus
extern "C" {
#endif
//#include "includes.h"
#include "../../../source3/include/includes.h"
#include "lib/registry/registry.h"
#include "lib/cmdline/popt_common.h"
#include "lib/events/events.h"
#include "system/time.h"
#include "../libcli/smbreadline/smbreadline.h"
#include "librpc/gen_ndr/ndr_security.h"
#include "../librpc/gen_ndr/winreg.h"
#include "lib/registry/tools/common.h"
#include "param/param.h"
#include "credentials.h"
#include "../librpc/gen_ndr/ndr_winreg_c.h"
#include "../librpc/gen_ndr/ndr_netlogon_c.h"
#include "lib/netapi/netapi.h"
#include "lib/netapi/netapi_net.h"
#include "../../../source3/rpc_client/rpc_client.h"
#include "../../../source3/rpc_client/cli_pipe.h"
//#include "../../../source3/include/client.h"
#include "../../../libcli/smb/smb_constants.h"
#include "../../../libcli/smb/smbXcli_base.h"
#include "../../../source3/libsmb/proto.h"
#include "../../../source3/utils/net_rpc_registry.c"

#ifdef __cplusplus
}
#endif
#include "../../common/session.hpp"
#include <sys/socket.h>
#include "utils/net.h"

#include <iostream>
struct rpc_client
{};
static NTSTATUS rpc_registry_enumerate_internal(struct net_context *c,
                        const struct dom_sid *domain_sid,
                        const char *domain_name,
                        struct cli_state *cli,
                        struct rpc_pipe_client *pipe_hnd,
                        TALLOC_CTX *mem_ctx,
                        int argc,
                        const char **argv )
{
    struct policy_handle pol_hive, pol_key;
    NTSTATUS status;
    WERROR werr;
    uint32_t num_subkeys = 0;
    uint32_t num_values = 0;
    char **names = NULL, **classes = NULL;
    NTTIME **modtimes = NULL;
    uint32_t i;
    struct registry_value **values = NULL;
    struct dcerpc_binding_handle *b = pipe_hnd->binding_handle;



    status = registry_openkey(mem_ctx, pipe_hnd, argv[0], REG_KEY_READ,
                  &pol_hive, &pol_key);
    if (!NT_STATUS_IS_OK(status)) {
        d_fprintf(stderr, _("registry_openkey failed: %s\n"),
              nt_errstr(status));
        return status;
    }

    status = registry_enumkeys(mem_ctx, pipe_hnd, &pol_key, &num_subkeys,
                   &names, &classes, &modtimes);
    if (!NT_STATUS_IS_OK(status)) {
        d_fprintf(stderr, _("enumerating keys failed: %s\n"),
              nt_errstr(status));
        return status;
    }

    for (i=0; i<num_subkeys; i++) {
        print_registry_key(names[i], modtimes[i]);
    }

    status = registry_enumvalues(mem_ctx, pipe_hnd, &pol_key, &num_values,
                     &names, &values);
    if (!NT_STATUS_IS_OK(status)) {
        d_fprintf(stderr, _("enumerating values failed: %s\n"),
              nt_errstr(status));
        return status;
    }

    for (i=0; i<num_values; i++) {
        print_registry_value_with_name(names[i], values[i]);
    }

    dcerpc_winreg_CloseKey(b, mem_ctx, &pol_key, &werr);
    dcerpc_winreg_CloseKey(b, mem_ctx, &pol_hive, &werr);

    return status;
}
int run_rpc_command(struct net_context *c,
            struct cli_state *cli_arg,
                const struct ndr_interface_table *table,
            int conn_flags,
            rpc_command_fn fn,
            int argc,
            const char **argv)
{
    struct cli_state *cli = NULL;
    struct rpc_pipe_client *pipe_hnd = NULL;
    TALLOC_CTX *mem_ctx;
    NTSTATUS nt_status;
    struct dom_sid *domain_sid;
    const char *domain_name;
    int ret = -1;

    /* make use of cli_state handed over as an argument, if possible */
    if (!cli_arg) {
        nt_status = net_make_ipc_connection(c, conn_flags, &cli);
        if (!NT_STATUS_IS_OK(nt_status)) {
            DEBUG(1, ("failed to make ipc connection: %s\n",
                  nt_errstr(nt_status)));
            return -1;
        }
    } else {
        cli = cli_arg;
    }

    if (!cli) {
        return -1;
    }

    /* Create mem_ctx */

    if (!(mem_ctx = talloc_init("run_rpc_command"))) {
        DEBUG(0, ("talloc_init() failed\n"));
        goto fail;
    }

    nt_status = net_get_remote_domain_sid(cli, mem_ctx, &domain_sid,
                          &domain_name);
    if (!NT_STATUS_IS_OK(nt_status)) {
        goto fail;
    }

    if (!(conn_flags & NET_FLAGS_NO_PIPE)) {
        if (lp_client_schannel()
            && (ndr_syntax_id_equal(&table->syntax_id,
                        &ndr_table_netlogon.syntax_id))) {
            /* Always try and create an schannel netlogon pipe. */
            TALLOC_FREE(c->netlogon_creds);
            nt_status = cli_rpc_pipe_open_schannel(
                cli, c->msg_ctx, table, NCACN_NP,
                domain_name,
                &pipe_hnd, c, &c->netlogon_creds);
            if (!NT_STATUS_IS_OK(nt_status)) {
                DEBUG(0, ("Could not initialise schannel netlogon pipe. Error was %s\n",
                    nt_errstr(nt_status) ));
                goto fail;
            }
        } else {
            if (conn_flags & NET_FLAGS_SEAL) {
                nt_status = cli_rpc_pipe_open_generic_auth(
                    cli, table,
                    (conn_flags & NET_FLAGS_TCP) ?
                    NCACN_IP_TCP : NCACN_NP,
                    CRED_DONT_USE_KERBEROS,
                    DCERPC_AUTH_TYPE_NTLMSSP,
                    DCERPC_AUTH_LEVEL_PRIVACY,
                    smbXcli_conn_remote_name(cli->conn),
                    lp_workgroup(), c->opt_user_name,
                    c->opt_password, &pipe_hnd);
            } else {
                nt_status = cli_rpc_pipe_open_noauth(
                    cli, table,
                    &pipe_hnd);
            }
            if (!NT_STATUS_IS_OK(nt_status)) {
                DEBUG(0, ("Could not initialise pipe %s. Error was %s\n",
                      table->name,
                    nt_errstr(nt_status) ));
                goto fail;
            }
        }
    }

    nt_status = rpc_registry_enumerate_internal(c, domain_sid, domain_name, cli, pipe_hnd, mem_ctx, argc, argv);

    if (!NT_STATUS_IS_OK(nt_status)) {
        DEBUG(1, ("rpc command function failed! (%s)\n", nt_errstr(nt_status)));
    } else {
        ret = 0;
        DEBUG(5, ("rpc command function succedded\n"));
    }

    if (!(conn_flags & NET_FLAGS_NO_PIPE)) {
        if (pipe_hnd) {
            TALLOC_FREE(pipe_hnd);
        }
    }

fail:
    /* close the connection only if it was opened here */
    if (!cli_arg) {
        cli_shutdown(cli);
    }

    talloc_destroy(mem_ctx);
    return ret;
}

int main(int argc, char **argv)
{

    trustwave::credentials creds1("%{domain}", "%{username}", "%{password}", "SSS");
        trustwave::session s("%{host}", creds1);

        TALLOC_CTX *frame = talloc_stackframe();
        struct net_context *c = talloc_zero(frame, struct net_context);
        //zero_sockaddr(&c->opt_dest_ip);
        if (!init_names())
                exit(1);

            load_interfaces();

            /* this makes sure that when we do things like call scripts,
               that it won't assert because we are not root */
            sec_init();

            if (c->opt_machine_pass) {
                /* it is very useful to be able to make ads queries as the
                   machine account for testing purposes and for domain leave */

                net_use_krb_machine_account(c);
            }

            if (!c->opt_password) {
                c->opt_password = getenv("PASSWD");
            }
/*
 * return run_rpc_command(c, NULL, &ndr_table_winreg, 0,
        rpc_registry_enumerate_internal, argc, argv );
}
 */


       auto  status = libnetapi_net_init(&c->netapi_ctx);
            if (status != 0) {
                return -1;
            }
            libnetapi_set_username(c->netapi_ctx, c->opt_user_name);
            libnetapi_set_password(c->netapi_ctx, c->opt_password);
            if (c->opt_kerberos) {
                libnetapi_set_use_kerberos(c->netapi_ctx);
            }
            if (c->opt_ccache) {
                libnetapi_set_use_ccache(c->netapi_ctx);
            }
            run_rpc_command(c,nullptr,&ndr_table_winreg, 0,rpc_registry_enumerate_internal,0,0);

   //     rc.enum1("SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion");
}

