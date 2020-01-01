//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														service_configuration.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 12/31/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_SERVICE_CONFIGURATION_HPP
#define SRC_BACKEND_SERVICES_SERVICE_CONFIGURATION_HPP
#include <taocpp-json/include/tao/json/contrib/traits.hpp>
#include <taocpp-json/include/tao/json.hpp>

namespace trustwave{
    struct service_configuration {
        virtual ~service_configuration() = default;
        service_configuration(const service_configuration&) = default;
        service_configuration(service_configuration&&) = default;
        service_configuration& operator=(const service_configuration&) = default;
        service_configuration& operator=(service_configuration&&) = delete;
        service_configuration() = delete;
        const std::string_view name() const
        {
            return name_;
        }

    protected:

        explicit service_configuration(const std::string_view name) :
                name_(name)
        {
        }
        const std::string_view name_;
        tao::json::value json_value_;
    };
}

namespace tao::json {
    template<>
    struct traits<trustwave::service_configuration> : binding::object<

            TAO_JSON_BIND_REQUIRED("name", &trustwave::service_configuration::name) > {
    };
}

#endif //SRC_BACKEND_SERVICES_SERVICE_CONFIGURATION_HPP