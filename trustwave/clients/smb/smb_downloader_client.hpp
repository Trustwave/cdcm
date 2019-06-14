/*
 * smb_downloader_client.hpp
 *
 *  Created on: May 12, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_
#define TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_
#ifdef __cplusplus
#include <sys/param.h>
extern "C" {
#endif

#include "libsmbclient.h"
#ifdef __cplusplus
}
#endif

#include <string>
static constexpr uint16_t SMB_MAXPATHLEN = MAXPATHLEN;
static constexpr uint16_t RESUME_CHECK_SIZE = 512;
static constexpr uint16_t RESUME_DOWNLOAD_OFFSET = 1024;
static constexpr uint16_t RESUME_CHECK_OFFSET = RESUME_DOWNLOAD_OFFSET + RESUME_CHECK_SIZE;
static constexpr uint16_t SMB_DEFAULT_BLOCKSIZE = 64000;

namespace trustwave {
class session;
class smb_downloader_client
{
public:
    smb_downloader_client(){}
    ~smb_downloader_client(){}
    bool download(const session& sess,const char *base, const char *name, bool resume, bool toplevel,const  char *outfile);
private:
    SMBCCTX* create_smbctx();
    bool connect( const char *path);
    int remote_fd_ = -1;
    int local_fd_ = -1;
    off_t total_bytes_ = 0;
    struct stat localstat_;
    struct stat remotestat_;
    SMBCCTX *ctx_ = nullptr;

};
}
#endif /* TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_ */
