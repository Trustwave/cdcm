//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_mapper.hpp
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
#ifndef UTILS_FILE_MAPPER_HPP
#define UTILS_FILE_MAPPER_HPP

#include <string>
#include <memory>
#include <boost/icl/split_interval_map.hpp>
#include "file_reader_interface.hpp"
#include "bounded_chunk.hpp"

namespace trustwave {
    class file_mapper {
        typedef ::boost::icl::interval_set<size_t, std::less, bounded_chunk> chunks_map;
    public:
        explicit file_mapper(file_reader_interface & fr) : fr_(fr) {
            if (fr_.validate_open()) {
                allocated_size_ = fr_.file_size();
                data_.reset(new char[allocated_size_]);
                memset(data_.get(), 0, allocated_size_);
            }
        }


        template<typename P>
        bool map_chunk_by_pointer(const P *offset, size_t size) {
            auto diff = reinterpret_cast<const char *>(offset) - data_.get();
            if (diff >= 0) {
                return map_chunk(diff, size);
            }
            return false;
        }

        bool map_chunk(size_t offset, size_t size);

        inline bool in_bound(size_t offset, size_t size) {
            return size + offset <= allocated_size_;
        }

        inline char *data() {
            return data_.get();
        }

    private:
        [[nodiscard]] size_t minimum_read_size(size_t offset, size_t size) const;
        file_reader_interface& fr_;
        uintmax_t allocated_size_;
        std::unique_ptr<char[]> data_;
        chunks_map map_;
    };
}
#endif //UTILS_FILE_MAPPER_HPP