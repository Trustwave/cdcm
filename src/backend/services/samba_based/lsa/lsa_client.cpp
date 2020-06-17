//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														lsa_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/9/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif
#include "source3/include/includes.h"
#include "../librpc/gen_ndr/ndr_lsa_c.h"
#include "libcli/smb/smb_constants.h"
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
#include "lsa_client.hpp"
#include "session.hpp"
#include "../rpc/rpc_client.hpp"
#include "credentials.hpp"
#include "../common/security_descriptor_utils.hpp"
#include <sstream>
//#include "singleton_runner/authenticated_scan_server.hpp"
using trustwave::lsa_client;
using trustwave::session;
// using trustwave::result;

/* These values discovered by inspection */

security_descriptor* lsa_client::get_secdesc(const std::string& filename)
{
    uint16_t fnum = (uint16_t)-1;
    uint32_t desired_access = SEC_STD_READ_CONTROL | SEC_FLAG_SYSTEM_SECURITY;
    uint32_t sec_info = SECINFO_OWNER | SECINFO_GROUP | SECINFO_DACL;
    NTSTATUS status = cli_ntcreate(client_->cli(), filename.c_str(), 0, desired_access, 0,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, 0x0, 0x0, &fnum, nullptr);
    if(!NT_STATUS_IS_OK(status)) {
        printf("Failed to open %s: %s\n", filename.c_str(), nt_errstr(status));
        return nullptr;
    }

    struct security_descriptor* sd;
    status = cli_query_security_descriptor(client_->cli(), fnum, sec_info, talloc_tos(), &sd);

    if(!NT_STATUS_IS_OK(status)) {
        printf("Failed to get security descriptor: %s\n", nt_errstr(status));
        return nullptr;
    }
    return sd;
}

int lsa_client::cacl_dump(const std::string& filename)
{
    security_descriptor* sd = get_secdesc(filename);
    if(sd == nullptr) { return -1; }
    std::stringstream ss;
    trustwave::sd_utils::sec_desc_print(client_->cli(), ss, sd,sd_utils::entity_type::NTFS_DIR);
    std::cerr << ss.str();
    std::cerr << trustwave::sd_utils::get_sd_str(client_->cli(), sd,sd_utils::entity_type::NTFS_DIR);
    return 0;
}
std::vector<trustwave::sd_utils::ACE_str> lsa_client::get_acls(const std::string& filename)
{
    security_descriptor* sd = get_secdesc(filename);
    if(sd == nullptr) { return std::vector<trustwave::sd_utils::ACE_str>(); }
    return trustwave::sd_utils::get_acls(client_->cli(), sd,sd_utils::entity_type::NTFS_DIR);
}

lsa_client::lsa_client():mem_ctx_(talloc_stackframe()), client_(std::make_unique<rpc_client>(mem_ctx_)) {  }
lsa_client::~lsa_client()
{
    talloc_free(mem_ctx_);
}
int lsa_client::connect(const session& sess, const std::string& share)
{
    client_->connect(sess, share,"?????", &ndr_table_lsarpc);
    return 0;
}