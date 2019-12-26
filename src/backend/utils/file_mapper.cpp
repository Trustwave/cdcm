//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_mapper.cpp
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
#include "file_mapper.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"

using namespace trustwave;

file_mapper::file_mapper(file_reader_interface & fr) : fr_(fr) {
    if (fr_.validate_open()) {
        allocated_size_ = fr_.file_size();
        data_.reset(new char[allocated_size_]);
        memset(data_.get(), 0, allocated_size_);
    }
}

bool file_mapper::map_chunk(size_t offset, size_t size) {
    if (in_bound(offset, size)) {
        bounded_chunk new_chunk(offset, offset + minimum_read_size(offset, size));
        auto chunk_iter = map_.find(new_chunk);
        if (chunk_iter == map_.end() || !chunk_iter->contains(new_chunk)) {
            if (fr_.read(new_chunk.offset(), new_chunk.size(), data_.get() + new_chunk.offset())) {
                map_.add(new_chunk);
                return true;
            } else {
                return false;
            }
        }
        else {
            return true;
        }
    }
    return false;
}

size_t file_mapper::minimum_read_size(size_t offset, size_t size) const {
    static const auto mrs = authenticated_scan_server::instance().settings.smb_.minimum_read_size_;
    return std::min(allocated_size_ - offset, std::max(mrs, size));
}