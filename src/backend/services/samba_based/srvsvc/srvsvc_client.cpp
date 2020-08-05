//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														srvsvc_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/18/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif
#include "source3/include/includes.h"
#include "../librpc/gen_ndr/ndr_srvsvc_c.h"
#include "../librpc/gen_ndr/lsa.h"
#include "libcli/smb/smb_constants.h"
#include "source3/libsmb/proto.h"
#include "source3/include/client.h"
#include "libcli/smb/smbXcli_base.h"
#include "source3/lib/netapi/netapi.h"
#include "libcli/security/dom_sid.h"
#include "source3/lib/netapi/netapi.h"
//#include "source3/rpc_client/cli_lsarpc.h"
//#include "libcli/security/security.h"
#include "source3/rpc_client/cli_pipe.h"
#ifdef __cplusplus
}
#endif
#include "srvsvc_client.hpp"
#include "session.hpp"
#include "../rpc/rpc_client.hpp"
#include "credentials.hpp"
#undef Required
#include "singleton_runner/authenticated_scan_server.hpp"
#define Required (3)
#include <sstream>
#include "../utils/security_descriptor_utils.hpp"
using trustwave::srvsvc_client;
using trustwave::session;
using trustwave::result;

srvsvc_client::srvsvc_client(): client_(std::make_unique<rpc_client>()) { }
srvsvc_client::~srvsvc_client() { }
result srvsvc_client::connect(const session& sess)
{
    return client_->connect_and_open_pipe(sess, "IPC$", "IPC", &ndr_table_srvsvc, true);
}
result srvsvc_client::enumerate_all_shares(std::vector<share_info>& shares_result)
{
    static constexpr uint32_t info_level = 2;
    struct srvsvc_NetShareInfoCtr info_ctr;
    ZERO_STRUCT(info_ctr);
    struct srvsvc_NetShareCtr2 ctr2;
    ZERO_STRUCT(ctr2);
    info_ctr.level = info_level;
    info_ctr.ctr.ctr2 = &ctr2;
    struct dcerpc_binding_handle* b = client_->pipe_handle()->binding_handle;
    uint32_t preferred_len = 0xffffffff;
    uint32_t totalentries = 0;
    WERROR result;
    NTSTATUS status = dcerpc_srvsvc_NetShareEnumAll(b, talloc_tos(), client_->pipe_handle()->desthost, &info_ctr,
                                                    preferred_len, &totalentries, nullptr, &result);

    if(!NT_STATUS_IS_OK(status)) { return {false, ntstatus_to_werror(status)}; }
    if(!W_ERROR_IS_OK(result)) { return {false, result}; }

    uint32_t count = info_ctr.ctr.ctr2->count;
    for(uint32_t i = 0; i < count; i++) {
        auto& element = info_ctr.ctr.ctr2->array[i];
        shares_result.push_back({element.name, element.comment, element.path});
    }
    return {true, ntstatus_to_werror(status)};
}

std::ostream& trustwave::operator<<(std::ostream& os, const trustwave::share_info& si)
{
    return os << "Name: " << si.name_ << "\n"
              << "\tComment: " << si.comment_ << "\n"
              << "\tPath: " << si.path_ << "\n";
}
