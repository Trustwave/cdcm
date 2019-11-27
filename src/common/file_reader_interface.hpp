//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_reader_interface.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/27/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_FILE_READER_INTERFACE_HPP
#define SRC_COMMON_FILE_READER_INTERFACE_HPP

#include <cstddef>
#include <cstdint>
namespace trustwave {
    class file_reader_interface {
    public:
        virtual bool read(size_t offset, size_t size, char *dest) = 0;

        [[nodiscard]] virtual uintmax_t file_size() const = 0;

        [[nodiscard]] virtual bool validate_open() = 0;

        virtual ~file_reader_interface()=default;
    };
}
#endif //SRC_COMMON_FILE_READER_INTERFACE_HPP