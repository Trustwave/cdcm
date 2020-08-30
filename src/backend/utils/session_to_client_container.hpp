//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														session_to_client_container.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/27/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_UTILS_SESSION_TO_CLIENT_CONTAINER_HPP
#define SRC_BACKEND_UTILS_SESSION_TO_CLIENT_CONTAINER_HPP
#include <boost/asio.hpp>
#include "singleton_runner/process_specific_object.hpp"
#include "singleton_runner/clients_deleter.hpp"
#include "dispatcher.hpp"
#include "clients_map.hpp"

namespace trustwave{
    class session;
    class sessions_to_clients : public process_specific_object, public clients_deleter
    {
        static Dispatcher<process_specific_object>::Registrator m_registrator;
    public:

        static constexpr std::string_view name{"sessions_to_clients"};
        sessions_to_clients(boost::asio::io_context& ios): process_specific_object(name),ios_(ios) {}
        void get_clients_for_session(const std::string& session_id);
        trustwave::Dispatcher<trustwave::cdcm_client> & get_clients_dispatcher();
        void set_client(std::shared_ptr<cdcm_client> c,const std::string& session_id,size_t idle_client_timeout);
        template <typename T>
        std::shared_ptr<trustwave::cdcm_client> get_client(const std::string& session_id)
        {
            auto it = session_to_clients_.find(session_id);
            if(it != session_to_clients_.end())
            {
                return it->second->get_client<T>();
            }
            return std::make_shared<T>();
        }
        void erase(const std::string& session_id)
        {
            session_to_clients_.erase(session_id);
        }
    private:
        std::map<std::string,std::unique_ptr<clients_map>> session_to_clients_;
        boost::asio::io_context& ios_;
    };
}
#endif // SRC_BACKEND_UTILS_SESSION_TO_CLIENT_CONTAINER_HPP