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

#ifndef TRUSTWAVE_COMMON_SESSION_HPP_
#define TRUSTWAVE_COMMON_SESSION_HPP_
#include "credentials.hpp" // for credentials
#include <array>
#include <boost/uuid/string_generator.hpp> // for uuid
#include <boost/uuid/uuid.hpp> // for uuid
#include <memory>
#include <map>
#include <string> // for string
#include "client.hpp"

namespace trustwave {

    class cdcm_client;
    class session final {
    public:
        session();
        session(std::string remote, const credentials& creds);
        const boost::uuids::uuid& id() const;
        std::string idstr() const;

        const std::string& remote() const;
        credentials creds() const;

        explicit operator bool() const { return uuid_.is_nil(); }

        bool id(const std::string& ids) //rotem: assaf, can i move to cpp?
        {
            try {
                uuid_ = boost::uuids::string_generator()(ids);
            }
            catch(std::runtime_error& ex) {
                return false;
            }
            return true;
        }

        void remote(const std::string& dest) { remote_ = dest; }

        void creds(const std::string& domain, const std::string& username, const std::string& password,
                   const std::string& workstation)
        {
            creds_.creds(domain, username, password, workstation);
        }

    private:
        boost::uuids::uuid uuid_;
        std::string remote_;
        credentials creds_;
//        clients_map* clients_;
    };
} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_SESSION_HPP_ */
