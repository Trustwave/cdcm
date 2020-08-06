//===========================================================================
// Trustwave ltd. @{SRCH}
//								client.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 8 Jul 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_CLIENT_HPP_
#define TRUSTWAVE_COMMON_CLIENT_HPP_
#include <string_view>
namespace trustwave {

    class cdcm_client {
    public:
        virtual ~cdcm_client() = default;

    protected:
        cdcm_client(const std::string_view proto):protocol_(proto)
        {}
        const std::string_view protocol_;
    };

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_CLIENT_HPP_ */
