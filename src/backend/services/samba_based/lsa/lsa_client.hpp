//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														lsa_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/9/20
// Comments:
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_LSA_CLIENT_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_LSA_CLIENT_HPP
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================

#ifdef __cplusplus
extern "C" {
#endif
#include "includes.h"
#include "lib/events/events.h"
#include "param/param.h"
#include "credentials.h"
#ifdef __cplusplus
}
#endif
#undef uint_t
#include <memory>
#include <string>
#include <tuple>
#include "client.hpp"
#include "configurable.hpp"
#include "../utils/security_descriptor_utils.hpp"
#include "../rpc/rpc_client.hpp"

struct security_descriptor;

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    class session;
    using result = std::tuple<bool, WERROR>;
    class lsa_client final: public cdcm_client{
  public:
        //fixme assaf add copy ctor move ......
        lsa_client();
        ~lsa_client() override = default;
        result connect(const session& sess,const std::string& share);
        result get_sd(const std::string& path,sd_utils::entity_type et,trustwave::sd_utils::Security_Descriptor_str &outsd);

        result cacl_dump(const std::string& filename);
    private:
        result get_acls(const std::string& filename,std::vector<sd_utils::ACE_str>&);
        result get_secdesc(const std::string& filename,security_descriptor*&);
        std::unique_ptr<rpc_client> client_;
    };
} // namespace trustwave

#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_LSA_CLIENT_HPP