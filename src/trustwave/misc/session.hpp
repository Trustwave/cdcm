//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														session.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 19 Apr 2019
// Comments:

#ifndef TRUSTWAVE_MISC_SESSION_HPP_
#define TRUSTWAVE_MISC_SESSION_HPP_
#include <boost/uuid/uuid.hpp>             // for uuid
#include <string>                          // for string
#include "credentials.hpp"                 // for credentials
struct cli_credentials;

namespace trustwave {

class session
{
public:
    session();
    session(const std::string& remote, const credentials& creds);
    const boost::uuids::uuid& id() const;
    std::string idstr() const;

    const std::string& remote() const;
    cli_credentials* creds() const;
    explicit operator bool() const
    {
        return uuid_.is_nil();
    }
private:
    boost::uuids::uuid uuid_;
    std::string remote_;
    credentials creds_;
};
}

#endif /* TRUSTWAVE_MISC_SESSION_HPP_ */
