//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														pe_context.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/21/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef UTILS_PE_CONTEXT_HPP
#define UTILS_PE_CONTEXT_HPP

#include <string>
#include <unordered_set>
#include "file_mapper.hpp"
#include "libpe/pe.h"
#include "libpe/resources.h"
#include "libpe/dir_resources.h"

namespace trustwave {
    class pe_context final {
    public:
        explicit pe_context(file_reader_interface& fr): fm_(fr), pe_() {}
        ~pe_context()
        {
            free(pe_.directories);
            free(pe_.sections);
        }
        [[nodiscard]] int parse();
        void extract_info(std::map<std::u16string, std::u16string>&, const std::unordered_set<std::u16string>& s);

    private:
        uint64_t pe_rva2ofs(uint64_t rva);
        IMAGE_DATA_DIRECTORY* pe_directory_by_entry(ImageDirectoryEntry entry);
        NODE_PERES* discoveryNodesPeres();

    private:
        file_mapper fm_;
        pe_file_t pe_;
    };
} // namespace trustwave
#endif // UTILS_PE_CONTEXT_HPP