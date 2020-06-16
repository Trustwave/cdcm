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
#include "../rpc/rpc_client.hpp"

namespace trustwave{

    /**
 * Split path into hive name and subkeyname
 * normalizations performed:
 *  - if the path contains no '\\' characters,
 *    assume that the legacy format of using '/'
 *    as a separator is used and  convert '/' to '\\'
 *  - strip trailing '\\' chars
 */
 

    class session;
    class registry_value;
    class enum_key;
    struct reg_context
    {
        struct policy_handle pol_hive, pol_key;
    };
    using result = std::tuple<bool, WERROR>;
    static constexpr auto werr_pipe_busy = 0xe7;
    class registry_client2 final: public cdcm_client/*, public configurable<registry_service_configuration> */{
        struct key_info {
            const char* classname = nullptr;
            uint32_t num_subkeys = 0, max_subkeylen = 0; //, max_classlen;
            NTTIME last_changed_time = 0;
            uint32_t num_values = 0, max_valnamelen = 0, max_valbufsize = 0;
        };

    public:
        registry_client2() :client_(std::make_unique<rpc_client>()) { mem_ctx_ = talloc_stackframe(); }
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
        NTSTATUS get_sd();
    private:

        result key_get_info(key_info&);
        void normalize(registry_value& rv);

        std::unique_ptr<rpc_client> client_;
        reg_context ctx_;
//        DATA_BLOB data_blob_;
        TALLOC_CTX* mem_ctx_;
    };
}
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_REGISTRY_REGISTRY_CLIENT2_HPP