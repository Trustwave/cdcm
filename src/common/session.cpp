//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														session.cpp
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

#include "session.hpp"

#include <boost/uuid/nil_generator.hpp> // for nil_uuid
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <boost/uuid/random_generator.hpp> // for random_generator
#pragma GCC diagnostic pop

#include <boost/uuid/uuid_io.hpp> // for to_string
#include <utility>

using namespace trustwave;

session::session(): uuid_(boost::uuids::nil_uuid()), creds_(std::string(), std::string(), std::string(), std::string())
{
}

session::session(std::string remote, const credentials& creds):
    uuid_(boost::uuids::random_generator()()), remote_(std::move(remote)), creds_(creds)
{
}
std::string session::idstr() const { return boost::uuids::to_string(uuid_); }
const boost::uuids::uuid& session::id() const { return uuid_; }
const std::string& session::remote() const { return remote_; }
credentials session::creds() const { return creds_; }
