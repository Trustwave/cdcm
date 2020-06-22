//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														srvsvc_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/18/20
// Comments:
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_SRVSVC_CLIENT_HPP
#define SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_SRVSVC_CLIENT_HPP
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
struct security_descriptor;

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    class session;
    class rpc_client;
    using result = std::tuple<bool, WERROR>;
    struct share_info
    {
        std::string name_;
        std::string comment_;
        std::string path_;
    };
    class srvsvc_client final: public cdcm_client{
    public:
        //fixme assaf add copy ctor move ......
        srvsvc_client();
        ~srvsvc_client() override;
        result connect(const session& sess);
        result enumerate_all_shares(std::vector<share_info>&);
    private:
        std::unique_ptr<rpc_client> client_;
    };
    std::ostream& operator<<(std::ostream& , const trustwave::share_info& );
} // namespace trustwave
#endif // SRC_BACKEND_SERVICES_SAMBA_BASED_SRVSVC_SRVSVC_CLIENT_HPP