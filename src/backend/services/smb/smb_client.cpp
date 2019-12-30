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
#ifdef __cplusplus
}
#endif
#undef uint_t

#include "singleton_runner/authenticated_scan_server.hpp"
#include "session.hpp"
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

static void smbc_auth_fn(const char *pServer, const char *, char *pWorkgroup, int maxLenWorkgroup,
                         char *pUsername, int maxLenUsername, char *pPassword, int maxLenPassword) {
    auto sess = authenticated_scan_server::instance().sessions->get_session_by<shared_mem_sessions_cache::remote>(
            std::string(pServer));
    //  AU_LOG_DEBUG("server is %s ", pServer);
    static int krb5_set = 1;
    const char *wg = "WORKGROUP";
    if (!sess->id().is_nil()) {

        AU_LOG_INFO("smbc_auth_fn session for %s found", pServer);
        strncpy(pWorkgroup, wg, static_cast<size_t>(maxLenWorkgroup - 1));
        strncpy(pUsername, sess->creds().username_.c_str(), static_cast<size_t>(maxLenUsername - 1));
        strncpy(pPassword, sess->creds().password_.c_str(), static_cast<size_t>(maxLenPassword - 1));
        AU_LOG_INFO("smbc_auth_fn session for %s found and set", pServer);
        return;
    }

    if (krb5_set && getenv("KRB5CCNAME")) {
        krb5_set = 0;
        return;
    }

    krb5_set = 1;
}


std::pair<bool,int> smb_client::connect(const char *path) {
    AU_LOG_ERROR("path: %s", path);
    if (smbc_init(smbc_auth_fn, 1) < 0) {
        AU_LOG_ERROR("Unable to initialize libsmbclient");
        return std::make_pair(false,-1);
    }
    remote_fd_ = smbc_open(path, O_RDONLY, 0755);

    current_open_path_ =path;
    if (remote_fd_ <= 0) {
        return std::make_pair(false,errno);
    }
    if (smbc_fstat(remote_fd_, &remotestat_) < 0) {
        AU_LOG_ERROR("Can't stat %s: %s", path, strerror(errno));
        return std::make_pair(false,-1);
    }
    return std::make_pair(true,0);
}

bool smb_client::download(const char *base, const char *name, bool resume,
                          bool toplevel, const char *outfile) {
    char path[SMB_MAXPATHLEN];
    snprintf(path, SMB_MAXPATHLEN - 1, "%s%s%s", base,
             (*base && *name && name[0] != '/' && base[strlen(base) - 1] != '/') ? "/" : "", name);
    if (!connect(path).first) {
        return false;
    }
    char checkbuf[2][RESUME_CHECK_SIZE];

    off_t offset_download = 0, offset_check = 0, curpos = 0;

    const char *newpath;
    if (outfile) {
        newpath = outfile;
    } else if (!name[0]) {
        newpath = strrchr(base, '/');
        if (newpath) {
            newpath++;
        } else {
            newpath = base;
        }
    } else {
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
                    "been retrieved", newpath, (intmax_t) offset_check, (intmax_t) localstat_.st_size,
                    (intmax_t) remotestat_.st_size);
    }

    if (offset_check) {
        off_t off1, off2;
        /* First, check all bytes from offset_check to
         * offset_download */
        off1 = lseek(local_fd_, offset_check, SEEK_SET);
        if (off1 < 0) {
            AU_LOG_ERROR("Can't seek to %jd in local file %s", (intmax_t) offset_check, newpath);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        off2 = smbc_lseek(remote_fd_, offset_check, SEEK_SET);
        if (off2 < 0) {
            AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t) offset_check, newpath);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }

        if (off1 != off2) {
            AU_LOG_ERROR("Offset in local and remote "
                         "files are different "
                         "(local: %jd, remote: %jd)", (intmax_t) off1, (intmax_t) off2);
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

        if (::read(local_fd_, checkbuf[1], RESUME_CHECK_SIZE) != RESUME_CHECK_SIZE) {
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
                        "offset %jd", (intmax_t) offset_download);
        } else {
            AU_LOG_ERROR("Local and remote file appear "
                         "to be different, not "
                         "doing resume for %s", path);
            smbc_close(remote_fd_);
            close(local_fd_);
            return false;
        }
    }
    download_portion(curpos, remotestat_.st_size, true);


    smbc_close(remote_fd_);
    close(local_fd_);
    return true;
}
static constexpr off_t max_mem_segment=512*1024*1024;//fixme assaf make it configurable

bool smb_client::download_portion(off_t curpos, off_t count, bool to_file)
{
    char *readbuf = new char[SMB_DEFAULT_BLOCKSIZE];
    /* Now, download all bytes from offset_download to the end */
    for (; curpos < count; curpos += SMB_DEFAULT_BLOCKSIZE) {
        ssize_t bytesread=0;
        ssize_t byteswritten=0;

        bytesread = smbc_read(remote_fd_, readbuf, SMB_DEFAULT_BLOCKSIZE);
        if (bytesread < 0) {
            AU_LOG_ERROR("Can't read %d bytes at offset %jd, file %s", SMB_DEFAULT_BLOCKSIZE, (intmax_t) curpos,
                         current_open_path_.data());
            smbc_close(remote_fd_);
            if (local_fd_ != STDOUT_FILENO) {
                close(local_fd_);
            }
            delete[] readbuf;
            return false;
        }

        if(to_file)
        {
            byteswritten = write(local_fd_, readbuf, static_cast<size_t>(bytesread));
        } else{
            byteswritten+=bytesread;
        }
        if (byteswritten != bytesread) {
            AU_LOG_ERROR("Can't write %zd bytes to local file %s at "
                         "offset %jd", bytesread, current_open_path_.data(), (intmax_t) curpos);
            delete[] readbuf;
            smbc_close(remote_fd_);
            if (local_fd_ != STDOUT_FILENO) {//fixme assaf change condition to -1
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
bool smb_client::download_portion_to_memory(const char *base, const char *name,off_t offset = 0, off_t count=max_mem_segment)
{
    char path[SMB_MAXPATHLEN];
    snprintf(path, SMB_MAXPATHLEN - 1, "%s%s%s", base,
             (*base && *name && name[0] != '/' && base[strlen(base) - 1] != '/') ? "/" : "", name);
    if (!connect(path).first) {
        return false;
    }
    off_t off = smbc_lseek(remote_fd_, offset, SEEK_SET);
    if (off < 0) {
        AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t) offset, path);
        smbc_close(remote_fd_);
        close(local_fd_);
        return false;
    }
    return download_portion(off, off + count, false);



}
bool smb_client::list(const std::string &path,std::vector<trustwave::dirent> &dirents) {
    int dh1, dsize, dirc;
    char dirbuf[SMB_DEFAULT_BLOCKSIZE];
    char *dirp;
    if (smbc_init(smbc_auth_fn, 1) < 0) {
        AU_LOG_ERROR("Unable to initialize libsmbclient");
        return false;
    }
    if ((dh1 = smbc_opendir(path.c_str())) < 1) {
        AU_LOG_ERROR( "Could not open directory: %s: %s\n",
                path.c_str(), strerror(errno));

        return false;
    }
    dirp = (char *) dirbuf;
    if ((dirc = smbc_getdents(static_cast<unsigned int>(dh1), (struct smbc_dirent *) dirp,
                              sizeof(dirbuf))) < 0) {

        AU_LOG_ERROR( "Problems getting directory entries: %s\n",
                strerror(errno));
        return false;
    }
    while (dirc > 0) {
        dsize = ((struct smbc_dirent *) dirp)->dirlen;
        if(((struct smbc_dirent *) dirp)->smbc_type == 7 || ((struct smbc_dirent *) dirp)->smbc_type==8) {
            dirents.push_back(
                    { std::string(((struct smbc_dirent *) dirp)->name ),((struct smbc_dirent *) dirp)->smbc_type==7 ? std::string("DIR"):std::string("FILE")});
        }
        dirp += dsize;
        dirc -= dsize;
    }
    return true;
}

ssize_t smb_client::read(size_t offset, size_t size, char *dest)
{
    off_t off = smbc_lseek(remote_fd_, offset, SEEK_SET);
    if (off < 0) {
        AU_LOG_ERROR("Can't seek to %jd in remote file %s", (intmax_t) offset, current_open_path_.data());
        smbc_close(remote_fd_);
        close(local_fd_);
        return -1;
    }
    /* Now, download all bytes from offset_download to size */
    size_t curpos=0;
    ssize_t bytesread = 1;
    while ( curpos < size && bytesread > 0) {
        bytesread = smbc_read(remote_fd_, dest+curpos, size-curpos);
        if (bytesread < 0) {
            AU_LOG_ERROR("Can't read %d bytes at offset %jd, file %s", SMB_DEFAULT_BLOCKSIZE, (intmax_t) curpos,
                         current_open_path_.data());
            smbc_close(remote_fd_);
            return -1;
        }
        curpos += bytesread;
    }
    return curpos;
}
uintmax_t smb_client::file_size() const
{
    return static_cast<uintmax_t>(remotestat_.st_size);
}
time_t smb_client::last_modified() const
{
    return remotestat_.st_mtim.tv_sec;
}
bool smb_client::validate_open()
{
    return connect(current_open_path_.data()).first;
}
