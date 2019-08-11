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
class credentials;
}
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
    typedef boost::interprocess::allocator <void, boost::interprocess::managed_shared_memory::segment_manager> void_allocator;

    static sp_session_t convert(const shared_mem_session &sm);
    static shared_mem_session convert(const sp_session_t &uc, const void_allocator &va);
};

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_CONVERTERS_HPP_ */
