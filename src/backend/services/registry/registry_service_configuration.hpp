//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_service_configuration.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 1/1/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_REGISTRY_REGISTRY_SERVICE_CONFIGURATION_HPP
#define SRC_BACKEND_SERVICES_REGISTRY_REGISTRY_SERVICE_CONFIGURATION_HPP

#include "configuration.hpp"
namespace trustwave {
    struct registry_service_configuration: public configuration {
        static constexpr std::string_view srv_name{"registry"};
        registry_service_configuration(): configuration(srv_name) {}
        off_t data_blob_size = 1024 * 1024;
        size_t reconnect_attempt_on_pipe_busy = 1;
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::registry_service_configuration>:
        binding::object<binding::inherit<traits<trustwave::configuration>>,
                        TAO_JSON_BIND_OPTIONAL("data_blob_size",
                                               &trustwave::registry_service_configuration::data_blob_size)> {
        // TAO_JSON_DEFAULT_KEY(trustwave::registry_service_configuration::srv_name.data());

        template<template<typename...> class Traits>
        static trustwave::registry_service_configuration as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::registry_service_configuration result;
            const auto o = v.at(trustwave::registry_service_configuration::srv_name);
            result.data_blob_size = o.template optional<off_t>("data_blob_size").value_or(result.data_blob_size);

            return result;
        }
    };
} // namespace tao::json
#endif // SRC_BACKEND_SERVICES_REGISTRY_REGISTRY_SERVICE_CONFIGURATION_HPP