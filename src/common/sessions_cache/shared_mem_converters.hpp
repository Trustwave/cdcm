//===========================================================================
// Trustwave ltd. @{SRCH}
//								session_converter.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 Jul 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_CONVERTERS_HPP_
#define TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_CONVERTERS_HPP_
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
//===================================================================
//                      Forward declarations
//===================================================================
namespace trustwave {
    class shared_mem_session;
    class session;
    class shared_mem_credentials;
    struct credentials;
} // namespace trustwave
//===================================================================
//                          namespaces
//===================================================================
namespace trustwave {
    using sp_session_t = boost::shared_ptr<session>;
    /////////////////////////////////////////////////////////////////////
    //
    // Class        :   UC_Converter
    // Created on   :   Dec 18, 2012
    // Author       :   ACOHEN
    // Description  :
    //
    //////////////////////////////////////////////////////////////////////

    struct session_converter {
        using void_allocator
            = boost::interprocess::allocator<void, boost::interprocess::managed_shared_memory::segment_manager>;

        static sp_session_t convert(const shared_mem_session&);
        static shared_mem_session convert(const sp_session_t&, const void_allocator&);
    };

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_CONVERTERS_HPP_ */
