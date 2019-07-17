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
extern "C" {
#endif
#include "auth/credentials/credentials.h"
#ifdef __cplusplus
}
#endif

#include "../common/credentials.hpp"
using namespace trustwave;
credentials::credentials(const std::string& domain, const std::string& username, const std::string& password,
                const std::string workstation)
{
    creds(domain, username, password, workstation);
}
credentials::~credentials()
{
    if (creds_) {
//fixme assaf release allocated memory
    }
}
cli_credentials *credentials::creds() const
{
    auto ret = ::cli_credentials_init(NULL);
    cli_credentials_set_domain(ret, cli_credentials_get_domain(creds_), CRED_SPECIFIED);
    cli_credentials_set_username(ret, cli_credentials_get_username(creds_), CRED_SPECIFIED);
    cli_credentials_set_password(ret, cli_credentials_get_password(creds_), CRED_SPECIFIED);
    cli_credentials_set_workstation(ret, cli_credentials_get_workstation(creds_), CRED_SPECIFIED);
    return ret;
}
void credentials::creds(const std::string& domain, const std::string& username, const std::string& password,
                const std::string workstation)
{
    if (creds_) {
        //fixme assaf release allocated memory
       }
    creds_ = ::cli_credentials_init(NULL);
    cli_credentials_set_domain(creds_, domain.c_str(), CRED_SPECIFIED);
    cli_credentials_set_username(creds_, username.c_str(), CRED_SPECIFIED);
    cli_credentials_set_password(creds_, password.c_str(), CRED_SPECIFIED);
    cli_credentials_set_workstation(creds_, workstation.c_str(), CRED_SPECIFIED);
}
