//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														winrm_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 1/7/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_WINRM_CLIENT_WINRM_CLIENT_HPP
#define SRC_BACKEND_SERVICES_WINRM_CLIENT_WINRM_CLIENT_HPP

#include "IWsmanClient.h"
#include "Exception.h"
#include "WsmanFilter.h"
#include "WsmanOptions.h"

struct _WsManClient;
typedef struct _WsManClient WsManClient; // FW declaration of struct
struct WsManClientData;

namespace trustwave
{
    class winrm_client final : public cdcm_client,public configurable<winrm_service_configuration>
    {
    private:
        WsManClient* cl;

    public:
        // Construct from params.
        winrm_client(
                const std::string &host,
                const int port,
                const std::string &path,
                const std::string &scheme,
                const std::string &auth_method,
                const std::string &username,
                const std::string &password
        );
        winrm_client& operator =(const winrm_client& cl)= delete;
        winrm_client(const winrm_client& cl)= delete;

        // Destructor.
        virtual ~winrm_client() override ;

        // Identify.
        std::string Identify() const;

        // Enumerate resource.
        void Enumerate(
                const std::string &resourceUri,
                std::vector<std::string> &enumRes,
                const NameValuePairs *s) const;
        void Enumerate(
                const std::string &resourceUri,
                WsmanFilter & filter,
                std::vector<std::string> &enumRes) const;
        void Enumerate(
                const std::string &resourceUri,
                std::vector<std::string> &enumRes,
                const WsmanOptions &options,
                const WsmanFilter &filter ) const;
        std::string Get(
                const std::string &resourceUri,
                const WsmanOptions &options) const;
        std::string Get(
                const std::string &resourceUri,
                const NameValuePairs *s ) const;

        // Set auth method
        void SetAuth(const std::string &auth_method);

        // Set timeout method
        void SetTimeout(unsigned long mtime);
        // Set user name
        void SetUserName(const std::string &user_name);

        // Set passsword
        void SetPassword(const std::string &password);

        // Set encoding
        void SetEncoding(const std::string &encoding);

        // Set/Get CIM namespace
        void SetNamespace(const std::string &ns);
        std::string GetNamespace() const;
       // Set server certificate params
        void SetServerCert(
                const std::string &cainfo = std::string(),
                const std::string &capath = std::string());

        // Set client certificates params
        void SetClientCert(
                const std::string &cert,
                const std::string &key);
    };
} // namespace trustwave
#endif //SRC_BACKEND_SERVICES_WINRM_CLIENT_WINRM_CLIENT_HPP