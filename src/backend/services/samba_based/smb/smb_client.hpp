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
#include <vector>
#include "client.hpp"
#include "configurable.hpp"
#include "file_reader_interface.hpp"
#include "smb_service_configuration.hpp"
static constexpr uint16_t SMB_MAXPATHLEN = MAXPATHLEN;
static constexpr uint16_t RESUME_CHECK_SIZE = 512;
static constexpr uint16_t RESUME_DOWNLOAD_OFFSET = 1024;
static constexpr uint16_t RESUME_CHECK_OFFSET = RESUME_DOWNLOAD_OFFSET + RESUME_CHECK_SIZE;
static constexpr uint32_t SMB_DEFAULT_BLOCKSIZE = 1024 * 64;
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================

namespace trustwave {
    class session;
    struct dirent {
        std::string name_;
        std::string type_;
    };
    class smb_client final:
        public cdcm_client,
        public file_reader_interface,
        public configurable<smb_service_configuration> {
    public:
        static constexpr std::string_view protocol{"smb"};
        smb_client();
        ~smb_client() override;
        bool list_dir(const std::string& path, std::vector<trustwave::dirent>& dirents);
        bool download_portion_to_memory(const char* base, const char* name, off_t offset, off_t count);
        ssize_t read(size_t offset, size_t size, char* dest) override;
        [[nodiscard]] uintmax_t file_size() const override;
        [[nodiscard]] time_t last_modified() const;
        bool validate_open() override;
        std::pair<bool, int> open_file(const char* path);

    private:
        bool download_portion(off_t curpos, off_t count, bool to_file);
        int remote_fd_ = -1;
        int local_fd_ = -1;
        SMBCCTX* ctx_;
        SMBCCTX* old_;
        struct stat localstat_ {
        };
        struct stat remotestat_ {
        };
        std::string_view current_open_path_;
    };
} // namespace trustwave
#endif /* TRUSTWAVE_CLIENTS_SMB_SMB_DOWNLOADER_CLIENT_HPP_ */
