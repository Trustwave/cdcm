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

#ifdef __cplusplus
extern "C" {
#endif
#include "auth/credentials/credentials.h"
#include "libcli/registry/util_reg.h"
#include "lib/util/time.h"
#ifdef __cplusplus
}

#endif

#include "session.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../utils/registry_utils.hpp"

using trustwave::registry_client;
using trustwave::result;

registry_client::registry_client() : ctx_(nullptr), ev_ctx_(nullptr), data_blob_{} {
    if (this->init_conf(authenticated_scan_server::instance().service_conf_repository)) {
        AU_LOG_INFO("%s", conf_->to_string().c_str());
    }

    mem_ctx_ = talloc_new(nullptr);
    ctx_ = talloc_zero(mem_ctx_,
    struct reg_context);

    ev_ctx_ = s4_event_context_init(mem_ctx_);
    data_blob_ = data_blob_talloc_zero(mem_ctx_, conf_->data_blob_size);
}

registry_client::~registry_client() { talloc_free(mem_ctx_); }

result registry_client::connect(const session &sess) {
    AU_LOG_DEBUG(" ");
    auto creds = ::cli_credentials_init(mem_ctx_);
    cli_credentials_set_domain(creds, sess.creds().domain().c_str(), CRED_SPECIFIED);
    cli_credentials_set_username(creds, sess.creds().username().c_str(), CRED_SPECIFIED);
    cli_credentials_set_password(creds, sess.creds().password().c_str(), CRED_SPECIFIED);
    cli_credentials_set_workstation(creds, sess.creds().workstation().c_str(), CRED_SPECIFIED);
    WERROR error = reg_open_remote(mem_ctx_, &ctx_->registry, nullptr, creds, ::loadparm_init_global(false),
                                   sess.remote().c_str(), ev_ctx_);

    if (!W_ERROR_IS_OK(error)) { return {false, error}; }
    return {true, error};

}

result registry_client::open_key(const char *full_path) {
    AU_LOG_DEBUG(" ");
    uint32_t hive;
    std::string keyname;
    if (!trustwave::reg_hive_key(full_path, hive, keyname)) {
        return {false, ntstatus_to_werror(NT_STATUS_INVALID_PARAMETER)};
    }
    WERROR error = reg_get_predefined_key(ctx_->registry, hive, &ctx_->current);
    if (!W_ERROR_IS_OK(error)) {
        return {false, error};
    }
    ctx_->predef = talloc_strdup(ctx_, hive_long_names.at(hive).c_str());
    ctx_->path = talloc_strdup(ctx_, "");
    ctx_->root = ctx_->current;
    error = reg_open_key(ctx_->registry, ctx_->root, keyname.c_str(), &ctx_->current);
    if (!W_ERROR_IS_OK(error)) {
        AU_LOG_ERROR("open key failed '%s'", full_path);
        return {false, error};
    }
    return {true, error};
}

void registry_client::normalize(registry_value &rv) {
    AU_LOG_DEBUG("Type is: %d", rv.type());
    switch (rv.type()) {
        case REG_EXPAND_SZ:
        case REG_SZ: {
            const char **a = nullptr;
            std::string s;
            for (int i = 0; i < data_blob_.length; i = i + 2) {
                if (data_blob_.data[i] == 0) {
                    if (data_blob_.data[i + 2] == 0) {
                        break;
                    }
                } else {
                    s.push_back(data_blob_.data[i]);
                }
            }
            rv.value(s);
        }
        break;
        case REG_MULTI_SZ: {
            AU_LOG_DEBUG("Type is REG_MULTI_SZ");
            const char **a = nullptr;
            std::string s;
            for (int i = 0; i < data_blob_.length; i = i + 2) {
                if (data_blob_.data[i] == 0) {
                    s.append("\n");
                    if (data_blob_.data[i + 2] == 0) {
                        break;
                    }
                } else {
                    s.push_back(data_blob_.data[i]);
                }
            }
            rv.value(s);
        }
        break;
        default: {
            AU_LOG_DEBUG("%s %zu ", data_blob_.data, data_blob_.length);
            auto char_ptr_value = reg_val_data_string(ctx_, rv.type(), data_blob_);
            if (nullptr == char_ptr_value) {
                AU_LOG_DEBUG("Value is null");
                rv.value(std::string());
            } else {
                rv.value(char_ptr_value);
            }
        }
    }
}

result registry_client::key_get_value_by_index(uint32_t idx, const char **name, registry_value &rv) {
    AU_LOG_DEBUG(" ");
    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_index(ctx_, ctx_->current, idx, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {
        //   AU_LOG_ERROR("No such index '%z'", idx);
        return {false, error};
    }
    rv.type(type);
    normalize(rv);
    return {true, error};
}

result registry_client::key_get_value_by_name(const char *name, registry_value &rv) {
    AU_LOG_DEBUG(" ");
    uint32_t type;
    data_blob_clear(&data_blob_);
    WERROR error = reg_key_get_value_by_name(ctx_, ctx_->current, name, &type, &data_blob_);
    if (!W_ERROR_IS_OK(error)) {
        //    AU_LOG_ERROR("No such value '%s'", name);
        return {false, error};
    }
    rv.type(type);
    normalize(rv);
    return {true, error};
}

result registry_client::key_get_info(key_info &ki) {
    AU_LOG_DEBUG(" ");
    WERROR error = reg_key_get_info(ctx_, ctx_->current, std::addressof(ki.classname), std::addressof(ki.num_subkeys),
                                    std::addressof(ki.num_values), std::addressof(ki.last_changed_time),
                                    std::addressof(ki.max_subkeylen), std::addressof(ki.max_valnamelen),
                                    std::addressof(ki.max_valbufsize));
    if (!W_ERROR_IS_OK(error)) {
        //    AU_LOG_ERROR("No such value '%s'", name);
        return {false, error};
    }

    return {true, error};
}

result registry_client::enumerate_key(const std::string &key, enum_key &ek) {
    AU_LOG_DEBUG(" ");
    const char *nm = nullptr;
    const char *cnm = nullptr;
    NTTIME lm;
    auto status = open_key(key.c_str());
    key_info ki;

    if (std::get<0>(status)) {
        status = key_get_info(ki);

        if (std::get<0>(status)) {
            for (uint32_t i = 0; i < ki.num_subkeys; i++) {
                reg_key_get_subkey_by_index(ctx_, ctx_->current, i, &nm, &cnm, &lm);
                time_t lmt = nt_time_to_unix(lm);
                ek.sub_keys_.push_back({nm, cnm ? cnm : "None", std::ctime(&lmt)});
            }
            for (uint32_t i = 0; i < ki.num_values; i++) {
                registry_value rv;
                const char *name;
                key_get_value_by_index(i, &name, rv);
                rv.name(name);
                ek.registry_values_.push_back(rv);
            }
        }
    }
    return status;
}

result registry_client::enumerate_key_values(const std::string &key, enum_key_values &ek) {
    AU_LOG_DEBUG(" ");
    auto status = open_key(key.c_str());
    key_info ki;

    if (std::get<0>(status)) {
        status = key_get_info(ki);

        if (std::get<0>(status)) {
            for (uint32_t i = 0; i < ki.num_values; i++) {
                registry_value rv;
                const char *name;
                key_get_value_by_index(i, &name, rv);
                rv.name(name);
                ek.registry_values_.push_back(rv);
            }
        }
    }
    return status;
}
