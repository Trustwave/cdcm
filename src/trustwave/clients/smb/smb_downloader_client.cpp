//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														smb_downloader_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 13 Apr 2019
// Comments:

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "smb_downloader_client.hpp"
#ifdef __cplusplus
extern "C" {
#endif
#include "includes.h"
#include "system/filesys.h"
#include "libsmbclient.h"
#include "system/time.h"
#include "credentials.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#include "../../common/singleton_runner/authenticated_scan_server.hpp"
#include "../../common/session.hpp"
/**@ingroup callback
 * Authentication callback function type (traditional method)
 *
 * Type for the the authentication function called by the library to
 * obtain authentication credentials
 *
 * For kerberos support the function should just be called without
 * prompting the user for credentials. Which means a simple 'return'
 * should work. Take a look at examples/libsmbclient/get_auth_data_fn.h
 * and examples/libsmbclient/testbrowse.c.
 *
 * @param srv       Server being authenticated to
 *
 * @param shr       Share being authenticated to
 *
 * @param wg        Pointer to buffer containing a "hint" for the
 *                  workgroup to be authenticated.  Should be filled in
 *                  with the correct workgroup if the hint is wrong.
 *
 * @param wglen     The size of the workgroup buffer in bytes
 *
 * @param un        Pointer to buffer containing a "hint" for the
 *                  user name to be use for authentication. Should be
 *                  filled in with the correct workgroup if the hint is
 *                  wrong.
 *
 * @param unlen     The size of the username buffer in bytes
 *
 * @param pw        Pointer to buffer containing to which password
 *                  copied
 *
 * @param pwlen     The size of the password buffer in bytes
 *
 */

using namespace trustwave;
static void smbc_auth_fn(const char * pServer, const char * pShare, char * pWorkgroup, int maxLenWorkgroup,
                char * pUsername, int maxLenUsername, char * pPassword, int maxLenPassword)
{
    auto sess = authenticated_scan_server::instance().sessions->get_session_by<shared_mem_sessions_cache::remote>(std::string(pServer));
  //  AU_LOG_DEBUG("server is %s ", pServer);
    static int krb5_set = 1;
    const char* wg = "WORKGROUP";
    if (!sess->id().is_nil()) {

        AU_LOG_INFO("smbc_auth_fn session for %s found", pServer);
        strncpy(pWorkgroup, wg, maxLenWorkgroup - 1);
        strncpy(pUsername, cli_credentials_get_username(sess->creds()), maxLenUsername - 1);
        strncpy(pPassword, cli_credentials_get_password(sess->creds()), maxLenPassword - 1);
        return;
    }

    if (krb5_set && getenv("KRB5CCNAME")) {
        krb5_set = 0;
        return;
    }

    krb5_set = 1;
}

SMBCCTX* smb_downloader_client::create_smbctx()
{
    SMBCCTX *ctx = nullptr;

    if ((ctx = smbc_new_context()) == nullptr)
        return nullptr;

    smbc_setDebug(ctx, /*debuglevel*/1);
    smbc_setFunctionAuthData(ctx, smbc_auth_fn);

    if (smbc_init_context(ctx) == nullptr) {
        smbc_free_context(ctx, 1);
        return NULL;
    }

    return ctx;
}
bool smb_downloader_client::connect(const char *path)
{
    ctx_ = create_smbctx();
    if (smbc_init(smbc_auth_fn, 1) < 0) {
        AU_LOG_ERROR("Unable to initialize libsmbclient");
        return true;
    }
    remote_fd_ = smbc_open(path, O_RDONLY, 0755);

    if (remote_fd_ < 0) {
        switch (errno)
        {
        case EISDIR:

            AU_LOG_ERROR("%s is a directory", path);
            return false;

        case ENOENT:
            AU_LOG_ERROR("%s can't be found on the remote server", path);
            return false;

        case ENOMEM:
            AU_LOG_ERROR("Not enough memory");
            return false;

        case ENODEV:
            AU_LOG_ERROR("The share name used in %s does not exist", path);
            return false;

        case EACCES:
            AU_LOG_ERROR("You don't have enough permissions "
                            "to access %s", path);
            return false;

        default:
            AU_LOG_ERROR("unknown smbc_open error");
            return false;
        }
    }
    if (smbc_fstat(remote_fd_, &remotestat_) < 0) {
        AU_LOG_ERROR("Can't stat %s: %s", path, strerror(errno));
        return false;
    }
    return true;
}

bool smb_downloader_client::download( const char *base, const char *name, bool resume,
                bool toplevel, const char *outfile)
{
    char path[SMB_MAXPATHLEN];
    snprintf(path, SMB_MAXPATHLEN - 1, "%s%s%s", base,
                    (*base && *name && name[0] != '/' && base[strlen(base) - 1] != '/') ? "/" : "", name);
    if (!connect(path)) {
        return false;
    }
    char checkbuf[2][RESUME_CHECK_SIZE];
    char *readbuf = nullptr;
    off_t offset_download = 0, offset_check = 0, curpos = 0;

    const char *newpath;
    if (outfile) {
        newpath = outfile;
    }
    else if (!name[0]) {
        newpath = strrchr(base, '/');
        if (newpath) {
            newpath++;
        }
        else {
            newpath = base;
        }
    }
    else {
        newpath = name;
    }

    if (!toplevel && (newpath[0] == '/')) {
        newpath++;
    }

    local_fd_ = open(newpath, O_CREAT | O_NONBLOCK | O_RDWR | (!resume ? O_EXCL : 0), 0755);
    if (local_fd_ < 0) {
        AU_LOG_ERROR("Can't open %s: %s", newpath, strerror(errno));
        smbc_close(remote_fd_);
        return false;
    }

    if (fstat(local_fd_, &localstat_) != 0) {
        AU_LOG_ERROR("Can't fstat %s: %s", newpath, strerror(errno));
        smbc_close(remote_fd_);
        close(local_fd_);
        return false;
    }

    if (localstat_.st_size && localstat_.st_size == remotestat_.st_size) {
        AU_LOG_ERROR("%s is already downloaded "
                        "completely.", path);
        AU_LOG_ERROR("%s", path);
        smbc_close(remote_fd_);
        close(local_fd_);
        return true;
    }

    if (localstat_.st_size > RESUME_CHECK_OFFSET && remotestat_.st_size > RESUME_CHECK_OFFSET) {
        offset_download = localstat_.st_size - RESUME_DOWNLOAD_OFFSET;
        offset_check = localstat_.st_size - RESUME_CHECK_OFFSET;
        AU_LOG_INFO("Trying to start resume of %s at %jd"
                        "At the moment %jd of %jd bytes have "
                        "been retrieved", newpath, (intmax_t ) offset_check, (intmax_t ) localstat_.st_size,
                        (intmax_t ) remotestat_.st_size);
    }

    if (offset_check) {
        off_t off1, off2;
        /* First, check all bytes from offset_check to
         * offset_download */
        off1 = lseek(local_fd_, offset_check, SEEK_SET);
        if (off1 < 0) {
            AU_LOG_ERROR("Can't seek to %jd in local file %s", (intmax_t ) offset_check, newpath);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        off2 = smbc_lseek(remote_fd_, offset_check, SEEK_SET);
        if (off2 < 0) {
            AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t ) offset_check, newpath);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        if (off1 != off2) {
            AU_LOG_ERROR("Offset in local and remote "
                            "files are different "
                            "(local: %jd, remote: %jd)", (intmax_t ) off1, (intmax_t ) off2);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        if (smbc_read(remote_fd_, checkbuf[0], RESUME_CHECK_SIZE) != RESUME_CHECK_SIZE) {
            AU_LOG_ERROR("Can't read %d bytes from "
                            "remote file %s", RESUME_CHECK_SIZE, path);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        if (read(local_fd_, checkbuf[1], RESUME_CHECK_SIZE) != RESUME_CHECK_SIZE) {
            AU_LOG_ERROR("Can't read %d bytes from "
                            "local file %s", RESUME_CHECK_SIZE, name);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        if (memcmp(checkbuf[0], checkbuf[1], RESUME_CHECK_SIZE) == 0) {
            AU_LOG_INFO("Current local and remote file "
                            "appear to be the same. "
                            "Starting download from "
                            "offset %jd", (intmax_t ) offset_download);
        }
        else {
            AU_LOG_ERROR("Local and remote file appear "
                            "to be different, not "
                            "doing resume for %s", path);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }
    }

    readbuf = new char[SMB_DEFAULT_BLOCKSIZE];
    if (!readbuf) {
        AU_LOG_ERROR("Failed to allocate %zu bytes for read "
                        "buffer (%s)", SMB_DEFAULT_BLOCKSIZE, strerror(errno));
        if (local_fd_ != STDOUT_FILENO) {
            close(local_fd_);
        }
        return false;
    }

    /* Now, download all bytes from offset_download to the end */
    for (curpos = offset_download; curpos < remotestat_.st_size; curpos += SMB_DEFAULT_BLOCKSIZE) {
        ssize_t bytesread;
        ssize_t byteswritten;

        bytesread = smbc_read(remote_fd_, readbuf, SMB_DEFAULT_BLOCKSIZE);
        if (bytesread < 0) {
            AU_LOG_ERROR("Can't read %zu bytes at offset %jd, file %s", SMB_DEFAULT_BLOCKSIZE, (intmax_t ) curpos,
                            path);
            smbc_close(remote_fd_);
            if (local_fd_ != STDOUT_FILENO) {
                close(local_fd_);
            }
            delete[] readbuf;
            return false;
        }

        total_bytes_ += bytesread;

        byteswritten = write(local_fd_, readbuf, bytesread);
        if (byteswritten != bytesread) {
            AU_LOG_ERROR("Can't write %zd bytes to local file %s at "
                            "offset %jd", bytesread, path, (intmax_t ) curpos);
            delete[] readbuf;
            smbc_close(remote_fd_);
            if (local_fd_ != STDOUT_FILENO) {
                close(local_fd_);
            }
            return false;
        }
    }

    delete[] readbuf;

    smbc_close(remote_fd_);
    close(local_fd_);
    return true;
}
