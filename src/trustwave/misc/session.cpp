/*
 * session.cpp
 *
 *  Created on: Apr 29, 2019
 *      Author: root
 */

#include "session.hpp"
#include <boost/uuid/nil_generator.hpp>     // for nil_uuid
#include <boost/uuid/random_generator.hpp>  // for random_generator
#include <boost/uuid/uuid_io.hpp>           // for to_string
using namespace trustwave;

session::session() :
                uuid_(boost::uuids::nil_uuid()),
                creds_(std::string(),std::string(),std::string(),std::string())
{

}

session::session(const std::string& remote, const credentials& creds) :
                uuid_(boost::uuids::random_generator()()), remote_(remote), creds_(creds)
{

}
std::string session::idstr() const
{
    return boost::uuids::to_string(uuid_);

}
const boost::uuids::uuid& session::id() const
{
    return uuid_;
}
const std::string& session::remote() const
{
    return remote_;
}
struct cli_credentials* session::creds() const
{
    return creds_.creds();
}

