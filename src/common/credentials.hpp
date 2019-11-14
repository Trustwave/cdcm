//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														credentials.hpp
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

#ifndef TRUSTWAVE_COMMON_CREDENTIALS_HPP_
#define TRUSTWAVE_COMMON_CREDENTIALS_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <string>

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
struct credentials
{

    credentials(const std::string& domain, const std::string& username, const std::string& password,
                    const std::string workstation);

    ~credentials();
    void creds(const std::string& domain, const std::string& username, const std::string& password,
               const std::string workstation);
    std::string domain_;
    std::string username_;
    std::string password_;
    std::string workstation_;
};
}

#endif /* TRUSTWAVE_COMMON_CREDENTIALS_HPP_ */
