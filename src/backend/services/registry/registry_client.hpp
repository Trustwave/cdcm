//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 29 Apr 2019
// Comments:

#ifndef TRUSTWAVE_REGISTRY_CLIENT_HPP_
#define TRUSTWAVE_REGISTRY_CLIENT_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif
#include "includes.h"
#include "lib/registry/registry.h"
#include "lib/cmdline/popt_common.h"
#include "lib/events/events.h"
#include "system/time.h"
#include "libcli/smbreadline/smbreadline.h"
#include "librpc/gen_ndr/ndr_security.h"
#include "bin/default/librpc/gen_ndr/winreg.h"
#include "lib/registry/tools/common.h"
#include "param/param.h"
#include "credentials.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#include <string>
#include <tuple>
#include "client.hpp"
#include "registry_service_configuration.hpp"
#include "configurable.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    class session;
    class registry_value;
    class enum_key;
    struct reg_context {
        struct registry_context* registry;
        char* path;
        char* predef;
        struct registry_key* current;
        struct registry_key* root;
    };
    using result = std::tuple<bool, WERROR>;
    static constexpr auto werr_pipe_busy = 0xe7;
    class registry_client final: public cdcm_client, public configurable<registry_service_configuration> {
        struct key_info {
            const char* classname = nullptr;
            uint32_t num_subkeys = 0, max_subkeylen = 0; //, max_classlen;
            NTTIME last_changed_time = 0;
            uint32_t num_values = 0, max_valnamelen = 0, max_valbufsize = 0;
        };

    public:
        registry_client();
        ~registry_client() override;
        result connect(const session& sess);
        result key_get_value_by_name(const char* name, registry_value& rv);

        result key_get_value_by_index(uint32_t idx, const char** name, registry_value& rv);
        result key_get_subkey_by_index(TALLOC_CTX* mem_ctx, const struct registry_key* key, uint32_t idx,
                                       const char** name, const char** classname, NTTIME* last_mod_time);
        result key_get_subkey_by_name(TALLOC_CTX* mem_ctx, const struct registry_key* key, const char* name,
                                      struct registry_key** subkey);
        result enumerate_key(const std::string&, enum_key&);
        result key_exists(const std::string&);
        result value_exists(const char* valname);
        result open_key(const char* full_path);

    private:
        result key_get_info(key_info&);
        void normalize(registry_value& rv);


        reg_context* ctx_;
        tevent_context* ev_ctx_;
        DATA_BLOB data_blob_;
        TALLOC_CTX* mem_ctx_;
    };
} // namespace trustwave
#endif /* TRUSTWAVE_REGISTRY_CLIENT_HPP_ */
