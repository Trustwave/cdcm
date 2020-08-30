//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														clients_map.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/25/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "clients_map.hpp"
#include <iostream>
trustwave::clients_map::clients_factory trustwave::clients_map::clients_factory_;
void trustwave::clients_map::set_client(std::shared_ptr<cdcm_client> c,size_t idle_seconds_duration)
{

        std::scoped_lock lock(client_lock_);

        auto it = clients_.try_emplace(c->name(), timed_client(std::make_shared<boost::asio::steady_timer>(io_ctx_), c))
                      .first;
        it->second.timer_->expires_after(boost::asio::chrono::seconds(idle_seconds_duration));

        it->second.timer_->async_wait(
            boost::bind(&clients_map::dismiss_client, this, boost::asio::placeholders::error, c->name()));
        it->second.timer_->expires_at().time_since_epoch().count();
}
void trustwave::clients_map::dismiss_client(const boost::system::error_code& ec,const std::string_view protocol)
{
    if(ec.failed())
    {
        return;
    }
    std::scoped_lock lock(client_lock_);

    std::cerr<<"In dismiss: "<<protocol<<std::endl;
    auto it = clients_.find(protocol);
    if(it != clients_.end())
    {
        it->second.timer_->cancel();
        auto c = it->second.client_;
        deleter_.push(c);
        clients_.erase(it);
    }
}