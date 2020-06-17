//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														rpc_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 6/3/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_RPC_RPC_CLIENT_HPP
#define SRC_BACKEND_SERVICES_RPC_RPC_CLIENT_HPP
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifdef __cplusplus
extern "C" {
#endif
#include "libcli/util/error.h"
#include "lib/talloc/talloc.h"
#ifdef __cplusplus
}
#endif
#include <string>
#include <tuple>
#include "client.hpp"
#include "configurable.hpp"
struct cli_state;
struct cli_credentials;
struct rpc_pipe_client;
struct ndr_interface_table;

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    class session;
    using result = std::tuple<bool, WERROR>;
    class rpc_client final {
    public:
        //fixme assaf add copy ctor move ......
        rpc_client();
        ~rpc_client();
        result connect(const session& sess,const std::string& share,const std::string& device,const ndr_interface_table*);
        cli_state	*cli();
        rpc_pipe_client	*pipe_handle();
    private:
        cli_state	*cli_ = nullptr;
        cli_credentials* creds_= nullptr;
        rpc_pipe_client* pipe_handle_ = nullptr;
  };
} // namespace trustwave

#endif // SRC_BACKEND_SERVICES_RPC_RPC_CLIENT_HPP