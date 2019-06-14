/*
 * registry_client.cpp
 *
 *  Created on: Apr 22, 2019
 *      Author: root
 */

#include "registry_client.hpp"
#include "../../misc/session.hpp"
#include "registry_value.hpp"
#include "../../authenticated_scan_server.hpp"
using namespace trustwave;
registry_client::registry_client() :
                ctx_(nullptr), ev_ctx_(nullptr)
{
    const char *remote = NULL;
    ctx_ = talloc_zero(NULL, struct regshell_context);
    ev_ctx_ = s4_event_context_init(ctx_);
    data_blob_=data_blob_talloc_zero(nullptr,1024*1024);

}
registry_client::~registry_client()
{
    data_blob_clear_free(&data_blob_);

}
bool registry_client::connect(const session& sess,loadparm_context* lp_ctx)
{
    ctx_->registry = reg_common_open_remote(sess.remote().c_str(), ev_ctx_, lp_ctx, sess.creds());
    if (ctx_->registry != nullptr) {
        WERROR err;
        err = reg_get_predefined_key(ctx_->registry, reg_predefined_keys[2].handle, &ctx_->current);
        if (W_ERROR_IS_OK(err)) {
            ctx_->predef = talloc_strdup(ctx_, reg_predefined_keys[2].name);
            ctx_->path = talloc_strdup(ctx_, "");
            ctx_->root = ctx_->current;

            return true;
        }
        return false;
    }
    return false;
}
result registry_client::open_key(const char* full_path)
{

    WERROR error = reg_open_key(ctx_->registry, ctx_->root, full_path, &ctx_->current);
    if (!W_ERROR_IS_OK(error)) {
        AU_LOG_ERROR("open key failed '%s'", full_path);
        return {false,error};
    }
    return {true, error};
}

result registry_client::key_get_value_by_name(const char *name, registry_value& rv)
{

    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_name(ctx_, ctx_->current, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {
        AU_LOG_ERROR("No such value '%s'", name);
        return {false,error};

    }
    rv.type(type);
    rv.value(reg_val_data_string(ctx_, type, data_blob_));
    return {true,error};

}
//    result get_predefined_key_by_name(struct registry_context *ctx, const char *name, struct registry_key **key)
//    {
//
//    }
//    result get_predefined_key(struct registry_context *ctx, uint32_t hkey, struct registry_key **key);

result registry_client::key_get_value_by_index(TALLOC_CTX *mem_ctx, const struct registry_key *key, uint32_t idx, const char **name, registry_value& rv)
{
    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_index(ctx_, ctx_->current, idx, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {

        AU_LOG_ERROR("No such index '%z'", idx);
        return {false,error};

    }
    rv.type(type);
    rv.value(reg_val_data_string(ctx_, type, data_blob_));
    AU_LOG_ERROR("%s%s", str_regtype(type), reg_val_data_string(ctx_, type, data_blob_));
    return {true,error};
}
/*result key_get_info(TALLOC_CTX *mem_ctx, const struct registry_key *key, const char **class_name, uint32_t *num_subkeys, uint32_t *num_values, NTTIME *last_change_time, uint32_t *max_subkeynamelen, uint32_t *max_valnamelen, uint32_t *max_valbufsize);
 result key_get_subkey_by_index(TALLOC_CTX *mem_ctx, const struct registry_key *key, uint32_t idx, const char **name, const char **classname, NTTIME *last_mod_time);
 result key_get_subkey_by_name(TALLOC_CTX *mem_ctx, const struct registry_key *key, const char *name, struct registry_key **subkey);
 */
