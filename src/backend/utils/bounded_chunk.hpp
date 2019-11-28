//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														bounded_chunk.hpp
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
#ifndef UTILS_BOUNDED_CHUNK_HPP
#define UTILS_BOUNDED_CHUNK_HPP
namespace trustwave {
    class bounded_chunk {
    public:
        typedef size_t offset_type;
        bounded_chunk(offset_type off, offset_type s) : offset_(off), end_(s) {
        }

        [[nodiscard]] inline bool contains(const bounded_chunk &bc) const {
            return offset_ <= bc.offset_ && end() >= bc.end();
        }

        [[nodiscard]] inline offset_type offset() const {
            return offset_;
        }

        [[nodiscard]] inline offset_type size() const {
            return end_ - offset_;
        }

        [[nodiscard]] inline offset_type end() const {
            return end_;
        }

    private:
        offset_type offset_;
        offset_type end_;
    };
}
namespace boost::icl {
        template<>
        struct interval_traits<trustwave::bounded_chunk>
        {
            typedef trustwave::bounded_chunk interval_type;
            typedef interval_type::offset_type domain_type;
            typedef std::less<domain_type> domain_compare;
            static interval_type construct(const domain_type &lo, const domain_type &up) {
                return interval_type(lo, up);
            }
            static domain_type lower(const interval_type &inter_val) { return inter_val.offset(); };
            static domain_type upper(const interval_type &inter_val) { return inter_val.end(); };
        };
        template<>
        struct interval_bound_type<trustwave::bounded_chunk>
        {
            typedef interval_bound_type type;
            BOOST_STATIC_CONSTANT(bound_type, value = interval_bounds::static_right_open);
        };

} // namespace boost icl
#endif //UTILS_BOUNDED_CHUNK_HPP