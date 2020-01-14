//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														smb_service_configuration.hpp
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
#ifndef SRC_BACKEND_SERVICES_SMB_SMB_SERVICE_CONFIGURATION_HPP
#define SRC_BACKEND_SERVICES_SMB_SMB_SERVICE_CONFIGURATION_HPP
namespace trustwave
{
    struct smb_service_configuration : public configuration {
        static constexpr std::string_view srv_name{"smb"};
        smb_service_configuration() :
                configuration(srv_name) {
        }
        off_t max_mem_segment=128*1024*1024;

    };
}
namespace tao::json {
    template<>
    struct traits<trustwave::smb_service_configuration>
            : binding::object<binding::inherit<traits<trustwave::configuration> >,
              TAO_JSON_BIND_OPTIONAL("max_mem_segment", &trustwave::smb_service_configuration::max_mem_segment) > {
    //TAO_JSON_DEFAULT_KEY(trustwave::smb_service_configuration::srv_name.data());


    template< template< typename... > class Traits >
    static trustwave::smb_service_configuration as( const tao::json::basic_value< Traits >& v )
    {
        trustwave::smb_service_configuration result;
        const auto o = v.at(trustwave::smb_service_configuration::srv_name);
        result.max_mem_segment = o.template optional< off_t >( "max_mem_segment" ).value_or(result.max_mem_segment);

        return result;
    }
};
}
#endif //SRC_BACKEND_SERVICES_SMB_SMB_SERVICE_CONFIGURATION_HPP