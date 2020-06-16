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
#include "../common/security_descriptor_utils.hpp"

//#include "rpc_service_configuration.hpp"
struct cli_state;
struct cli_credentials;
struct rpc_pipe_client;
struct ndr_interface_table;
struct security_descriptor;

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    class session;
    class rpc_client;
    using result = std::tuple<bool, WERROR>;
    typedef char fstring[256];
    class lsa_client final: public cdcm_client/*, public configurable<rpc_service_configuration>*/ {


    public:
        //fixme assaf add copy ctor move ......
        lsa_client();
        ~lsa_client() override;
        int connect(const session& sess,const std::string& share);
        int cacl_dump(const std::string& filename);
        std::vector<sd_utils::ACE_str> get_acls(const std::string& filename);

    private:

        security_descriptor * get_secdesc(const std::string& filename);

        std::unique_ptr<rpc_client> client_;
        TALLOC_CTX* mem_ctx_;


    };
} // namespace trustwave

#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_LSA_LSA_CLIENT_HPP