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
#include <memory>
namespace trustwave {

    class cdcm_client {
    public:
        virtual ~cdcm_client() = default;
        virtual std::shared_ptr<cdcm_client> clone()
        {
            return nullptr;
        }
        const std::string_view name() const
        {
            return protocol_;
        }
        bool connected()const
        {
            return connected_;
        }

    protected:
        explicit cdcm_client(const std::string_view proto):protocol_(proto)
        {}
        const std::string_view protocol_;
        bool connected_ = false;
    };

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_CLIENT_HPP_ */
