//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														winrm_service_configuration.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 1/5/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_WINRM_WINRM_SERVICE_CONFIGURATION_HPP
#define SRC_BACKEND_SERVICES_WINRM_WINRM_SERVICE_CONFIGURATION_HPP
namespace trustwave
{
    struct winrm_service_configuration : public configuration {
        static constexpr std::string_view srv_name{"winrm"};
        winrm_service_configuration() :
                configuration(srv_name) {
        }
        uint16_t service_port=5985;

    };
}
namespace tao::json {
    template<>
    struct traits<trustwave::winrm_service_configuration>
            : binding::object<binding::inherit<traits<trustwave::configuration> >,
              TAO_JSON_BIND_OPTIONAL("service_port", &trustwave::winrm_service_configuration::service_port) > {
    //TAO_JSON_DEFAULT_KEY(trustwave::winrm_service_configuration::srv_name.data());


    template< template< typename... > class Traits >
    static trustwave::winrm_service_configuration as( const tao::json::basic_value< Traits >& v )
    {
        trustwave::winrm_service_configuration result;
        const auto o = v.at(trustwave::winrm_service_configuration::srv_name);
        result.service_port = o.template optional< uint16_t >( "service_port" ).value_or(result.service_port);

        return result;
    }
};
}
#endif //SRC_BACKEND_SERVICES_WINRM_WINRM_SERVICE_CONFIGURATION_HPP