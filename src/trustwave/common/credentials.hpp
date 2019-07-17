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
struct cli_credentials;
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
class credentials
{
public:
    credentials(const std::string& domain, const std::string& username, const std::string& password,
                    const std::string workstation);

    ~credentials();

    cli_credentials *creds() const;
    void creds(const std::string& domain, const std::string& username, const std::string& password,
                    const std::string workstation);

private:
    struct cli_credentials *creds_;
};
}

#endif /* TRUSTWAVE_COMMON_CREDENTIALS_HPP_ */
