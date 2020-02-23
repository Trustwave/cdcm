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
#include "smb_client.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include "includes.h"
#include "system/filesys.h"
#include "libsmbclient.h"
#include "system/time.h"
#include "credentials.h"
#include "param.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#undef strcpy
#include "singleton_runner/authenticated_scan_server.hpp"
#include "session.hpp"
#include <taocpp-json/include/tao/json.hpp>
#include <taocpp-json/include/tao/json/contrib/traits.hpp>
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

using trustwave::smb_client;
namespace {
    void smbc_auth_fn(const char* pServer, const char*, char* pWorkgroup, int maxLenWorkgroup, char* pUsername,
                      int maxLenUsername, char* pPassword, int maxLenPassword)
    {
        auto sess = trustwave::authenticated_scan_server::instance()
                        .sessions->get_session_by<trustwave::shared_mem_sessions_cache::remote>(std::string(pServer));
        AU_LOG_DEBUG("server is %s ", pServer);
        static int krb5_set = 1;
        const char* wg = "WORKGROUP";
        if(!sess->id().is_nil()) {
            AU_LOG_INFO("smbc_auth_fn session for %s found", pServer);

            if(sess->creds().username().empty()) {
                strncpy(pWorkgroup, wg, static_cast<size_t>(maxLenWorkgroup - 1));
            }
            else {
                strncpy(pWorkgroup, sess->creds().domain().c_str(), static_cast<size_t>(maxLenWorkgroup - 1));
            }

            strncpy(pUsername, sess->creds().username().c_str(), static_cast<size_t>(maxLenUsername - 1));
            strncpy(pPassword, sess->creds().password().c_str(), static_cast<size_t>(maxLenPassword - 1));
            AU_LOG_INFO("smbc_auth_fn session for %s found and set", pServer);
            return;
        }

        if(krb5_set && getenv("KRB5CCNAME")) {
            krb5_set = 0;
            return;
        }

        krb5_set = 1;
    }

    SMBCCTX* create_smbctx(void)
    {
        SMBCCTX* ctx;

        if((ctx = smbc_new_context()) == nullptr) return nullptr;

        //smbc_setDebug(ctx, 100);
        smbc_setFunctionAuthData(ctx, smbc_auth_fn);
        if(smbc_init_context(ctx) == nullptr) {
            smbc_free_context(ctx, 1);
            return nullptr;
        }

        return ctx;
    }
    void delete_smbctx(SMBCCTX* ctx)
    {
        smbc_free_context(ctx, 0);
    }

} // namespace
smb_client::smb_client(): ctx_(nullptr)
{
    this->init_conf(authenticated_scan_server::instance().service_conf_reppsitory);
}
smb_client::~smb_client()
{
    smbc_close(remote_fd_);
    smbc_set_context(old_);
    delete_smbctx(ctx_);
    remote_fd_ = -1;
    close(local_fd_);
    local_fd_ = -1;
}

std::pair<bool, int> smb_client::open_file(const char* path)
{
    AU_LOG_DEBUG("path: %s", path);
    if(smbc_init(smbc_auth_fn, 0) < 0) {
        AU_LOG_ERROR("Unable to initialize libsmbclient");
        return std::make_pair(false, -1);
    }
    ctx_ = create_smbctx();
    old_ = smbc_set_context(ctx_);
    // smbc_setConfiguration(ctx_, smbc_getConfiguration(old_));

    remote_fd_ = smbc_open(path, O_RDONLY, 0755);
    current_open_path_ = path;
    if(remote_fd_ <= 0) {
        return std::make_pair(false, errno);
    }
    if(smbc_fstat(remote_fd_, &remotestat_) < 0) {
        AU_LOG_ERROR("Can't stat %s: %s", path, strerror(errno));
        return std::make_pair(false, -1);
    }
    return std::make_pair(true, 0);
}

bool smb_client::download_portion(off_t curpos, off_t count, bool to_file)
{
    char readbuf[SMB_DEFAULT_BLOCKSIZE];
    /* Now, download all bytes from offset_download to the end */
    for(; curpos < count; curpos += SMB_DEFAULT_BLOCKSIZE) {
        ssize_t bytesread = 0;
        ssize_t byteswritten = 0;

        bytesread = smbc_read(remote_fd_, readbuf, SMB_DEFAULT_BLOCKSIZE);
        if(bytesread < 0) {
            AU_LOG_ERROR("Can't read %d bytes at offset %jd, file %s", SMB_DEFAULT_BLOCKSIZE, (intmax_t)curpos,
                         current_open_path_.data());
            if(local_fd_ != STDOUT_FILENO) {
            }
            return false;
        }

        if(to_file) {
            byteswritten = write(local_fd_, readbuf, static_cast<size_t>(bytesread));
        }
        else {
            byteswritten += bytesread;
        }
        if(byteswritten != bytesread) {
            AU_LOG_ERROR("Can't write %zd bytes to local file %s at "
                         "offset %jd",
                         bytesread, current_open_path_.data(), (intmax_t)curpos);
            return false;
        }
    }
    return true;
}
bool smb_client::download_portion_to_memory(const char* base, const char* name, off_t offset = 0, off_t count = 0)
{
    char path[SMB_MAXPATHLEN];
    snprintf(path, SMB_MAXPATHLEN - 1, "%s%s%s", base,
             (*base && *name && name[0] != '/' && base[strlen(base) - 1] != '/') ? "/" : "", name);
    if(!open_file(path).first) {
        return false;
    }
    off_t off = smbc_lseek(remote_fd_, offset, SEEK_SET);
    if(off < 0) {
        AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t)offset, path);
        return false;
    }
    return download_portion(off, off + count, false);
}

bool smb_client::list_dir(const std::string& path, std::vector<trustwave::dirent>& dirents)
{
    int dh1, dsize, dirc;
    char* dirp;
    if(smbc_init(smbc_auth_fn, 100) < 0) {
        AU_LOG_ERROR("Unable to initialize libsmbclient");
        return false;
    }
    ctx_ = create_smbctx();
    old_ = smbc_set_context(ctx_);
    if((dh1 = smbc_opendir(path.c_str())) < 1) {
        AU_LOG_ERROR("Could not open directory: %s: %s\n", path.c_str(), strerror(errno));
        return false;
    }
    std::vector<std::unique_ptr<char[]>> bufs;
    bufs.emplace_back(std::make_unique<char[]>(SMB_DEFAULT_BLOCKSIZE));
    std::vector<char*>::size_type curr = 0;
    dirp = (char*)bufs[curr].get();
    while((dirc = smbc_getdents(static_cast<unsigned int>(dh1), (struct smbc_dirent*)dirp, SMB_DEFAULT_BLOCKSIZE))
          != 0) {
        if(dirc < 0) {
            AU_LOG_ERROR("Problems getting directory entries: %s\n", strerror(errno));
            return false;
        }
        while(dirc > 0) {
            dsize = ((struct smbc_dirent*)dirp)->dirlen;
            if(((struct smbc_dirent*)dirp)->smbc_type == 7 || ((struct smbc_dirent*)dirp)->smbc_type == 8) {
                dirents.push_back(
                    {std::string(((struct smbc_dirent*)dirp)->name),
                     ((struct smbc_dirent*)dirp)->smbc_type == 7 ? std::string("DIR") : std::string("FILE")});
            }
            dirp += dsize;
            dirc -= dsize;
        }
        bufs.emplace_back(std::make_unique<char[]>(SMB_DEFAULT_BLOCKSIZE));
        dirp = bufs[++curr].get();
    }
    smbc_closedir(dh1);
    return true;
}

ssize_t smb_client::read(size_t offset, size_t size, char* dest)
{
    off_t off = smbc_lseek(remote_fd_, offset, SEEK_SET);
    if(off < 0) {
        AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t)offset, current_open_path_.data());
        return -1;
    }
    /* Now, download all bytes from offset_download to size */
    size_t curpos = 0;
    ssize_t bytesread = 1;
    while(curpos < size && bytesread > 0) {
        bytesread = smbc_read(remote_fd_, dest + curpos, size - curpos);
        if(bytesread < 0) {
            AU_LOG_ERROR("Can't read %d bytes at offset %jd, file %s", SMB_DEFAULT_BLOCKSIZE, (intmax_t)curpos,
                         current_open_path_.data());
            return -1;
        }
        curpos += bytesread;
    }
    return curpos;
}
uintmax_t smb_client::file_size() const { return static_cast<uintmax_t>(remotestat_.st_size); }

time_t smb_client::last_modified() const { return remotestat_.st_mtim.tv_sec; }
bool smb_client::validate_open()
{
    if(ctx_ && !current_open_path_.empty()) {
        return true;
    }
    return open_file(current_open_path_.data()).first;
}
