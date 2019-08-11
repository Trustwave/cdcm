//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														smb_downloader_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 Jun 2019
// Comments:

#ifndef TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_
#define TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
#include <sys/param.h>
extern "C" {
#endif

#include "libsmbclient.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#include <string>
#include "../../common/client.hpp"
static constexpr uint16_t SMB_MAXPATHLEN = MAXPATHLEN;
static constexpr uint16_t RESUME_CHECK_SIZE = 512;
static constexpr uint16_t RESUME_DOWNLOAD_OFFSET = 1024;
static constexpr uint16_t RESUME_CHECK_OFFSET = RESUME_DOWNLOAD_OFFSET + RESUME_CHECK_SIZE;
static constexpr uint16_t SMB_DEFAULT_BLOCKSIZE = 64000;
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
class session;
class smb_downloader_client:public cdcm_client
{
public:
    smb_downloader_client()
    {
    }
    ~smb_downloader_client() override
    {
    }
    bool download( const char *base, const char *name, bool resume, bool toplevel,
                    const char *outfile);
private:
    SMBCCTX* create_smbctx();
    bool connect(const char *path);
    int remote_fd_ = -1;
    int local_fd_ = -1;
    off_t total_bytes_ = 0;
    struct stat localstat_;
    struct stat remotestat_;
    SMBCCTX *ctx_ = nullptr;

};
}
#endif /* TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_ */
