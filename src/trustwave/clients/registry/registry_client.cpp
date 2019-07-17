//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_client.cpp
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
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "registry_client.hpp"
#include "registry_value.hpp"
#include "../libcli/registry/util_reg.h"
#include "lib/util/time.h"
#include <ctime>

#include "../../common/session.hpp"

using namespace trustwave;
registry_client::registry_client() :
                ctx_(nullptr), ev_ctx_(nullptr)
{
    ctx_ = talloc_zero(NULL, struct regshell_context);

    ev_ctx_ = s4_event_context_init(ctx_);
    data_blob_ = data_blob_talloc_zero(nullptr, 1024 * 1024);//fixme assaf move to conf

}
registry_client::~registry_client()
{
    data_blob_clear_free(&data_blob_);
    TALLOC_FREE(ev_ctx_);
    TALLOC_FREE(ctx_);

}
bool registry_client::connect(const session& sess, loadparm_context* lp_ctx)
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
        //  AU_LOG_ERROR("open key failed '%s'", full_path);
        return {false,error};
    }
    return {true, error};
}

result registry_client::key_get_value_by_index(uint32_t idx, const char **name, registry_value& rv)
{
    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_index(ctx_, ctx_->current, idx, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {

        //   AU_LOG_ERROR("No such index '%z'", idx);
        return {false,error};

    }
    rv.type(type);
    rv.value(reg_val_data_string(ctx_, type, data_blob_));
    //   AU_LOG_ERROR("%s%s", str_regtype(type), reg_val_data_string(ctx_, type, data_blob_));
    return {true,error};
}
result registry_client::key_get_value_by_name(const char *name, registry_value& rv)
{

    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_name(ctx_, ctx_->current, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {
        //    AU_LOG_ERROR("No such value '%s'", name);
        return {false,error};

    }
    rv.type(type);
    rv.value(reg_val_data_string(ctx_, type, data_blob_));
    return {true,error};

}
result registry_client::key_get_info(key_info& ki)
{

    WERROR error = reg_key_get_info(ctx_, ctx_->current, std::addressof(ki.classname), std::addressof(ki.num_subkeys),
                    std::addressof(ki.num_values), std::addressof(ki.last_changed_time),
                    std::addressof(ki.max_subkeylen), std::addressof(ki.max_valnamelen),
                    std::addressof(ki.max_valbufsize));
    if (!W_ERROR_IS_OK(error)) {
        //    AU_LOG_ERROR("No such value '%s'", name);
        return {false,error};

    }

    return {true,error};

}

result registry_client::enumerate_key(const std::string& key, enum_key& ek)
{

    const char* nm = nullptr;
    const char* cnm = nullptr;
    NTTIME lm;
    auto status = open_key(key.c_str());
    key_info ki;

    if (std::get<0>(status)) {

        status = key_get_info(ki);

        if (std::get<0>(status)) {
            for (uint32_t i = 0; i < ki.num_subkeys; i++) {
                reg_key_get_subkey_by_index(ctx_, ctx_->current, i, &nm, &cnm, &lm);
                time_t lmt = nt_time_to_unix(lm);
                ek.sub_keyes_.push_back( { nm, cnm ? cnm : "None", std::ctime(&lmt) });

            }
            for (uint32_t i = 0; i < ki.num_values; i++) {
                registry_value rv;
                const char* name;
                key_get_value_by_index(i, &name, rv);
                rv.name(name);
                ek.registry_values_.push_back(rv);
            }
        }
    }
    return status;
}



