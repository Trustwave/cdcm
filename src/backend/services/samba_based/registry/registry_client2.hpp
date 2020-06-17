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

namespace trustwave{
    namespace sd_utils
    {
        struct Security_Descriptor_str;
    }
    class session;
    class registry_value;
    class enum_key;
    struct reg_context
    {
        struct policy_handle pol_hive, pol_key;
    };
    using result = std::tuple<bool, WERROR>;
    static constexpr auto werr_pipe_busy = 0xe7;
    class registry_client2 final: public cdcm_client{
//        struct key_info {
//            const char* classname = nullptr;
//            uint32_t num_subkeys = 0, max_subkeylen = 0; //, max_classlen;
//            NTTIME last_changed_time = 0;
//            uint32_t num_values = 0, max_valnamelen = 0, max_valbufsize = 0;
//        };

    public:
        registry_client2() :mem_ctx_( talloc_stackframe()),client_(std::make_unique<rpc_client>(mem_ctx_)) { }
        ~registry_client2() override;
        int connect(const session& sess);
//        result key_get_value_by_name(const char* name, registry_value& rv);
//        result key_get_value_by_index(uint32_t idx, const char** name, registry_value& rv);
//        result key_get_subkey_by_index(TALLOC_CTX* mem_ctx, const struct registry_key* key, uint32_t idx,
//                                       const char** name, const char** classname, NTTIME* last_mod_time);
//        result key_get_subkey_by_name(TALLOC_CTX* mem_ctx, const struct registry_key* key, const char* name,
//                                      struct registry_key** subkey);
//        result enumerate_key(const std::string&, enum_key&);
//        result key_exists(const std::string&);
//        result value_exists(const char* valname);
//
        NTSTATUS open_key(const std::string& k);
        NTSTATUS get_sd(sd_utils::Security_Descriptor_str&);
    private:

//        result key_get_info(key_info&);
//        void normalize(registry_value& rv);

        TALLOC_CTX* mem_ctx_;
        std::unique_ptr<rpc_client> client_;
        reg_context ctx_;
    };
}
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_REGISTRY_CLIENT2_HPP