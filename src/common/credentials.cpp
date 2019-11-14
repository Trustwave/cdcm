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
#include "credentials.hpp"


using namespace trustwave;
credentials::credentials(const std::string& domain, const std::string& username, const std::string& password,
                const std::string workstation):domain_(domain),username_(username),password_(password),workstation_(workstation)
{
}
credentials::~credentials()
{

}
void credentials::creds(const std::string& domain, const std::string& username, const std::string& password,
           const std::string workstation)
{
     domain_=domain;
     username_=username;
     password_=password;
     workstation_=workstation;
}

//cli_credentials *credentials::create_creds(const char* domain, const char* username, const char* password,
//                const char* workstation) const
//{
//    auto ret = ::cli_credentials_init(nullptr);
//    cli_credentials_set_domain(ret, domain, CRED_SPECIFIED);
//    cli_credentials_set_username(ret, username, CRED_SPECIFIED);
//    cli_credentials_set_password(ret, password, CRED_SPECIFIED);
//    cli_credentials_set_workstation(ret, workstation, CRED_SPECIFIED);
//    return ret;
//
//}
