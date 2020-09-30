//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_client2.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/15/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_REGISTRY_CLIENT2_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_REGISTRY_CLIENT2_HPP
#ifdef __cplusplus
extern "C" {
#endif
#include "source3/include/includes.h"
#include "../librpc/gen_ndr/misc.h"
#ifdef __cplusplus
}
#endif
#include "../rpc/rpc_client.hpp"

namespace trustwave {
    namespace sd_utils {
        struct Security_Descriptor_str;
    }
    class session;
    class registry_value;
    class enum_key;
    struct reg_context {
        struct policy_handle pol_hive, pol_key;
    };
    using result = std::tuple<bool, WERROR>;
    static constexpr auto werr_pipe_busy = 0xe7;
    class registry_client2 final: public cdcm_client {
    public:
        static constexpr std::string_view protocol{"smb_registry2"};
        registry_client2():cdcm_client(protocol), client_(std::make_unique<rpc_client>()) { }
        ~registry_client2() override = default;
        registry_client2(registry_client2 const& other) = delete;
        registry_client2& operator=(registry_client2 other) noexcept {
            swap(*this, other);
            return *this;
        }
        friend void swap(registry_client2& a,registry_client2& b) noexcept
        {
            using std::swap;
            swap(a.client_,b.client_);
        }
        result connect(const session& sess);
        result open_key(const std::string& k);
        result get_sd(sd_utils::Security_Descriptor_str&);

    private:
        std::unique_ptr<rpc_client> client_;
        reg_context ctx_;
    };
} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_REGISTRY_CLIENT2_HPP