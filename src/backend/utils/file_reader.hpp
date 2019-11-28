//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_reader.hpp
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
#ifndef UTILS_FILE_READER_HPP
#define UTILS_FILE_READER_HPP

#include "../../common/file_reader_interface.hpp"
#include <string>
#include <fstream>

namespace trustwave {
    class file_reader final: public file_reader_interface {
    public:

        explicit file_reader(const std::string &fname);
        ~file_reader()= default;
        ssize_t read(size_t offset, size_t size, char *dest) override ;
        uintmax_t file_size() const override ;
        bool validate_open() override ;
    private:
        std::string fname_;
        uintmax_t fsize_;
        std::ifstream stream_;
    };
}
#endif //UTILS_FILE_READER_HPP