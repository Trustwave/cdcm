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
#include <boost/python.hpp>
#include "registry_value.hpp"
namespace trustwave {
    class session;
    class wmi_registry_client final: public cdcm_client {

    public:
        wmi_registry_client()=default;
        ~wmi_registry_client() override = default;
        bool connect(const session& sess);
        bool key_get_value_by_name(const std::string&,const std::string&, registry_value& rv);
//        bool key_get_value_by_index(uint32_t idx, const char** name, registry_value& rv);
//        bool key_get_subkey_by_index(TALLOC_CTX* mem_ctx, const struct registry_key* key, uint32_t idx,
//                                       const char** name, const char** classname, NTTIME* last_mod_time);
//        bool key_get_subkey_by_name(TALLOC_CTX* mem_ctx, const struct registry_key* key, const char* name,
//                                      struct registry_key** subkey);
        bool enumerate_key(const std::string&, enum_key&);
        bool enumerate_key_values(const std::string&, enum_key_values&);
//        bool key_exists(const std::string&);
//        bool value_exists(const char* valname);

    private:
    using bpo = boost::python::object;
        bpo main_;
        bpo global_;
        bpo helper_;
        bpo exec_;

    };
} // namespace trustwave

#endif // SRC_BACKEND_SERVICES_WMI_REGISTRY_CLIENT_HPP