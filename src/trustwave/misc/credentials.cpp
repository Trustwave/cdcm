/*
 * credentials.cpp
 *
 *  Created on: Apr 29, 2019
 *      Author: root
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "auth/credentials/credentials.h"
#ifdef __cplusplus
}
#endif

#include "credentials.hpp"
using namespace trustwave;
credentials::credentials(const std::string& domain, const std::string& username, const std::string& password, const std::string workstation)
{
    creds_ = ::cli_credentials_init(NULL);
    cli_credentials_set_domain(creds_, domain.c_str(), CRED_SPECIFIED);
    cli_credentials_set_username(creds_, username.c_str(), CRED_SPECIFIED);
    cli_credentials_set_password(creds_, password.c_str(), CRED_SPECIFIED);
    cli_credentials_set_workstation(creds_, workstation.c_str(), CRED_SPECIFIED);
}

cli_credentials *credentials::creds() const
{
    return creds_;
}
