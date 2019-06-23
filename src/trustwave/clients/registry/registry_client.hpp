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
#include "../libcli/smbreadline/smbreadline.h"
#include "librpc/gen_ndr/ndr_security.h"
#include "lib/registry/tools/common.h"
#include "param/param.h"
#include "credentials.h"
#ifdef __cplusplus
}
#endif
#include <string>
#include <tuple>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
class session;
class registry_value;
struct regshell_context
{
    struct registry_context *registry;
    char *path;
    char *predef;
    struct registry_key *current;
    struct registry_key *root;
};
using result = std::tuple<bool,WERROR>;

class registry_client
{
public:
    registry_client();
    ~registry_client();
    bool connect(const session& sess, loadparm_context* lp_ctx);
    result open_key(const char* full_path);
    result key_get_value_by_name(const char *name, registry_value& rv);
    //    result get_predefined_key_by_name(struct registry_context *ctx, const char *name, struct registry_key **key)
//    {
//
//    }
//    result get_predefined_key(struct registry_context *ctx, uint32_t hkey, struct registry_key **key);

    result key_get_value_by_index(TALLOC_CTX *mem_ctx, const struct registry_key *key, uint32_t idx, const char **name,
                    registry_value& rv);
    result key_get_info(TALLOC_CTX *mem_ctx, const struct registry_key *key, const char **class_name,
                    uint32_t *num_subkeys, uint32_t *num_values, NTTIME *last_change_time, uint32_t *max_subkeynamelen,
                    uint32_t *max_valnamelen, uint32_t *max_valbufsize);
    result key_get_subkey_by_index(TALLOC_CTX *mem_ctx, const struct registry_key *key, uint32_t idx, const char **name,
                    const char **classname, NTTIME *last_mod_time);
    result key_get_subkey_by_name(TALLOC_CTX *mem_ctx, const struct registry_key *key, const char *name,
                    struct registry_key **subkey);
private:

    regshell_context *ctx_;
    tevent_context *ev_ctx_;
    DATA_BLOB data_blob_;

};
}
#endif /* TRUSTWAVE_REGISTRY_CLIENT_HPP_ */
