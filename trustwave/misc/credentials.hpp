/*
 * credentials.hpp
 *
 *  Created on: Apr 29, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_CREDENTIALS_HPP_
#define TRUSTWAVE_MISC_CREDENTIALS_HPP_

#include <string>
struct cli_credentials;
namespace trustwave{
class credentials
{
public:
    credentials(const std::string& domain, const std::string& username, const std::string& password,const std::string workstation);
    cli_credentials *creds() const;
private:
    struct cli_credentials *creds_;
};
}

#endif /* TRUSTWAVE_MISC_CREDENTIALS_HPP_ */
