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
    struct configuration {
        virtual ~configuration() = default;
        configuration(const configuration&) = default;
        configuration(configuration&&) = default;
        configuration& operator=(const configuration&) = default;
        configuration& operator=(configuration&&) = delete;
        configuration() = delete;
        const std::string_view name() const
        {
            return name_;
        }
        const std::string_view name_;
    protected:

        explicit configuration(const std::string_view name) :
                name_(name)
        {
        }
    };
}

namespace tao::json {
    template<>
    struct traits<trustwave::configuration> : binding::object<

            TAO_JSON_BIND_REQUIRED("name", &trustwave::configuration::name_) > {
    };
}

#endif //SRC_BACKEND_SERVICES_SERVICE_CONFIGURATION_HPP