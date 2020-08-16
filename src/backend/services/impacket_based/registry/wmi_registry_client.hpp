//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_registry_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 7/29/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_WMI_REGISTRY_CLIENT_HPP
#define SRC_BACKEND_SERVICES_WMI_REGISTRY_CLIENT_HPP
#include "client.hpp"
#include <string>
#include <tuple>
#include <boost/python.hpp>
#include "registry_value.hpp"
#include "../common/typedefs.hpp"

namespace trustwave {
    class session;
    class wmi_registry_client final: public cdcm_client {
    public:
        static constexpr std::string_view protocol{"wmi_registry"};
        wmi_registry_client():cdcm_client(protocol){}
        ~wmi_registry_client() override = default;
        impacket_based_common::result connect(const session& sess);
        impacket_based_common::result key_get_value_by_name(const std::string&,const std::string&, registry_value& rv);
        impacket_based_common::result enumerate_key(const std::string&, enum_key&);
        impacket_based_common::result enumerate_key_values(const std::string&, enum_key_values&);
        impacket_based_common::result key_exists(const std::string&,bool&);
        impacket_based_common::result value_exists(const std::string&,const std::string&,bool&);

    private:
        impacket_based_common::result internal_key_get_value_by_name(const std::string&,const std::string&, registry_value& rv);
    using bpo = boost::python::object;
        bpo main_;
        bpo global_;
        bpo helper_;
        bpo exec_;

    };
} // namespace trustwave

#endif // SRC_BACKEND_SERVICES_WMI_REGISTRY_CLIENT_HPP