//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														impacket_service_configuration.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/26/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_IMPACKET_SERVICE_CONFIGURATION_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_IMPACKET_SERVICE_CONFIGURATION_HPP
#include "configuration.hpp"

namespace trustwave {
    struct impacket_service_configuration: public configuration {
        static constexpr std::string_view name{"impacket"};
        impacket_service_configuration(): configuration(name) { }
        std::size_t idle_client_timeout = 60;
        std::string to_string() const override
        {
            std::stringstream ss;
            ss << "\n"
               << "\t" << name << " Configuration\n"
               << "\t"
               << "===============================\n"
               << "\tidle_client_timeout                          :\t" << idle_client_timeout << " bytes\n"
                ;
            return ss.str();
        }
    };
} // namespace trustwave
namespace tao::json {
    template<>
    struct traits<trustwave::impacket_service_configuration>:
        binding::object<binding::inherit<traits<trustwave::configuration>>,
        TAO_JSON_BIND_OPTIONAL("idle_client_timeout",
    &trustwave::impacket_service_configuration::idle_client_timeout)> {

    template<template<typename...> class Traits>
    static trustwave::impacket_service_configuration as(const tao::json::basic_value<Traits>& v)
    {
        trustwave::impacket_service_configuration result;
        const auto o = v.at(trustwave::impacket_service_configuration::name);
        result.idle_client_timeout = o.template optional<off_t>("idle_client_timeout").value_or(result.idle_client_timeout);

        return result;
    }
};
} // namespace tao::json
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_COMMON_IMPACKET_SERVICE_CONFIGURATION_HPP