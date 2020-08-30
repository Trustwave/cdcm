//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														clients_map.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/24/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_SINGLETON_RUNNER_CLIENTS_MAP_HPP
#define SRC_COMMON_SINGLETON_RUNNER_CLIENTS_MAP_HPP
#include <map>
#include <string>
#include <string_view>
#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "dispatcher.hpp"
#include "client.hpp"
#include "singleton_runner/clients_deleter.hpp"
#include <mutex>
namespace trustwave {
    class clients_map
    {
    public:
        explicit clients_map(boost::asio::io_service &ios, clients_deleter &deleter):io_ctx_(ios),deleter_(deleter){}
        void dismiss_client(const boost::system::error_code& ec,const std::string_view protocol);
        void set_client(std::shared_ptr<cdcm_client> c,size_t idle_seconds_duration);
        template <typename T>
        std::shared_ptr<cdcm_client> get_client()
        {
            std::scoped_lock lock(client_lock_);
            auto it = clients_.find(T::protocol);
            if(it == clients_.end())
            {
                return std::make_shared<T>();
            }
            it->second.timer_->cancel();
            auto rv = it->second.client_;
            clients_.erase(it);
            return rv;
        }
        static Dispatcher<cdcm_client>& dispatcher()
        {
            return clients_factory_.dispatcher();
        }
    private:
        boost::asio::io_service & io_ctx_;
        clients_deleter & deleter_;
        struct timed_client
        {
            timed_client(std::shared_ptr<boost::asio::steady_timer> t,std::shared_ptr<cdcm_client>c):timer_(t),client_(c)
            {}
            std::shared_ptr<boost::asio::steady_timer> timer_;
            std::shared_ptr<cdcm_client> client_;
        };
        std::map<std::string_view ,timed_client> clients_;
        std::mutex client_lock_;
        class clients_factory
        {
        public:
            using T_Ptr = std::shared_ptr<cdcm_client>;
            T_Ptr find(const std::string_view t_name) const
            {
                auto ret = clients_repository_.find(t_name);
                if(!ret) {
                    return ret;
                }
                return ret->clone();
            }
            T_Ptr find(const std::string& t_name) const
            {
                auto ret = clients_repository_.find(t_name);
                if(!ret) {
                    return ret;
                }
                return ret->clone();
            }

            Dispatcher<cdcm_client>& dispatcher()
            {
                return clients_repository_;
            }
        private:
            Dispatcher<cdcm_client> clients_repository_;
        };
        static clients_factory clients_factory_;
    };
}//namespace trustwave
#endif // SRC_COMMON_SINGLETON_RUNNER_CLIENTS_MAP_HPP