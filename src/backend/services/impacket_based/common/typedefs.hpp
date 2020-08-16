//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														typedefs.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/16/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_TYPEDEFS_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_TYPEDEFS_HPP
#include <tuple>
#include <string>
namespace trustwave{
    namespace impacket_based_common {
        using result = std::tuple<bool,std::string>;
    }//namespace impacket_based_common
}//namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_TYPEDEFS_HPP