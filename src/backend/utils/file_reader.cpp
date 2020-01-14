//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_reader.cpp
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
#include "file_reader.hpp"
#include <boost/filesystem.hpp>
#include <system_error>
#include <iostream>

using trustwave::file_reader;

file_reader::file_reader(const std::string &fname) : fname_(fname), stream_(fname, std::ios::out | std::ios::binary) {
    boost::system::error_code ec{};
    boost::filesystem::path p(fname_);
    fsize_ = boost::filesystem::file_size(p, ec);
}

ssize_t file_reader::read(size_t offset, size_t size, char *dest) {
    if (offset + size > fsize_) {
        return -1;
    }
    stream_.seekg(offset, std::ios_base::beg);
    if (!stream_.good()) {
        return -1;
    }
    stream_.read(dest, size);
    return stream_.good()?size:-1;
}

uintmax_t file_reader::file_size() const {
    return fsize_;
}

//bool file_reader::validate_open() const{
//    if (!stream_) {
//        return false;
//    }
//
//    return true;
//
//}