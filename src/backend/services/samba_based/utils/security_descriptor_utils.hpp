//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														security_descriptor_utils.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/15/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_SECURITY_DESCRIPTOR_UTILS_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_SECURITY_DESCRIPTOR_UTILS_HPP
#include <iosfwd>
#include <string>
#include <vector>

#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
struct cli_state;
struct security_descriptor;
struct security_ace;
namespace trustwave {
    namespace sd_utils {
        enum class entity_type {
            NTFS_FILE,
            NTFS_DIR,
            SHARE,
            REGISTRY,
            GENERIC

        };
        struct ACE_str {
            std::string SecurityPrincipal;
            std::vector<std::string> FileSystemRights;
            std::string AccessControlType;
            std::vector<std::string> AccessControlFlags;
        };
        struct Security_Descriptor_str {
            std::string Revision;
            std::vector<std::string> Control;
            std::string Owner;
            std::string Group;
            std::vector<ACE_str> ACLS;
        };

        std::vector<trustwave::sd_utils::ACE_str> get_acls(cli_state* cli, security_descriptor* sd, entity_type et);
        void sec_desc_print(cli_state* cli, std::stringstream& ss, security_descriptor* sd, entity_type et);
        Security_Descriptor_str get_sd_str(cli_state* cli, security_descriptor* sd, entity_type et);

    } // namespace sd_utils
    std::ostream& operator<<(std::ostream& os, const trustwave::sd_utils::ACE_str& acl);
    std::ostream& operator<<(std::ostream& os, const trustwave::sd_utils::Security_Descriptor_str&);
} // namespace trustwave

namespace tao ::json {
    template<>
    struct traits<trustwave::sd_utils::ACE_str>:
        binding::object<TAO_JSON_BIND_REQUIRED("SecurityPrincipal", &trustwave::sd_utils::ACE_str::SecurityPrincipal),
                        TAO_JSON_BIND_REQUIRED("FileSystemRights", &trustwave::sd_utils::ACE_str::FileSystemRights),
                        TAO_JSON_BIND_REQUIRED("AccessControlType", &trustwave::sd_utils::ACE_str::AccessControlType),
                        TAO_JSON_BIND_REQUIRED("AccessControlFlags", &trustwave::sd_utils::ACE_str::AccessControlFlags)

                        > {
    };
    template<>
    struct traits<trustwave::sd_utils::Security_Descriptor_str>:
        binding::object<TAO_JSON_BIND_REQUIRED("Revision", &trustwave::sd_utils::Security_Descriptor_str::Revision),
                        TAO_JSON_BIND_REQUIRED("Control", &trustwave::sd_utils::Security_Descriptor_str::Control),
                        TAO_JSON_BIND_REQUIRED("Owner", &trustwave::sd_utils::Security_Descriptor_str::Owner),
                        TAO_JSON_BIND_REQUIRED("Group", &trustwave::sd_utils::Security_Descriptor_str::Group),
                        TAO_JSON_BIND_REQUIRED("ACLS", &trustwave::sd_utils::Security_Descriptor_str::ACLS)

                        > {
    };

} // namespace tao::json
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_SECURITY_DESCRIPTOR_UTILS_HPP