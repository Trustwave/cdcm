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
void credentials::creds(const std::string& domain, const std::string& username, const std::string& password,
           const std::string& workstation)
{
     domain_=domain;
     username_=username;
     password_=password;
     workstation_=workstation;
}
