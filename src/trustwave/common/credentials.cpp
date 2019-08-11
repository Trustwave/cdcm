//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														credentials.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 29 Apr 2019
// Comments:

#ifdef __cplusplus
#include "credentials.hpp"
extern "C" {
#endif
#include "auth/credentials/credentials.h"
#ifdef __cplusplus
}
#endif

using namespace trustwave;
credentials::credentials(const std::string& domain, const std::string& username, const std::string& password,
                const std::string workstation)
{
    creds(domain, username, password, workstation);
}
credentials::~credentials()
{
    if (creds_){
        // talloc_free(creds_);
                // fixme assaf how to delete allocated memory

    }
}

cli_credentials *credentials::creds() const
{
    return create_creds(cli_credentials_get_domain(creds_), cli_credentials_get_username(creds_),
                    cli_credentials_get_password(creds_), cli_credentials_get_workstation(creds_));
}
cli_credentials *credentials::create_creds(const char* domain, const char* username, const char* password,
                const char* workstation) const
{
    auto ret = ::cli_credentials_init(nullptr);
    cli_credentials_set_domain(ret, domain, CRED_SPECIFIED);
    cli_credentials_set_username(ret, username, CRED_SPECIFIED);
    cli_credentials_set_password(ret, password, CRED_SPECIFIED);
    cli_credentials_set_workstation(ret, workstation, CRED_SPECIFIED);
    return ret;

}
void credentials::creds(const std::string& domain, const std::string& username, const std::string& password,
                const std::string workstation)
{
    if (creds_){
       // talloc_free(creds_);
        // fixme assaf how to delete allocated memory
    }
    creds_ = create_creds(domain.c_str(), username.c_str(), password.c_str(), workstation.c_str());
}
