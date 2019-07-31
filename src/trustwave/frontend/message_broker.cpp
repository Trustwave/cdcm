//===========================================================================
// Trustwave ltd. @{SRCH}
//								message_broker.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 26 Jun 2019
// Comments: 

//  ---------------------------------------------------------------------
//  Constructor for broker object
#include "frontend/message_broker.hpp"

#include <zmq.hpp>
#include<iostream>

#include "../common/client.hpp"
#include "../common/protocol/protocol.hpp"
#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/mdp.hpp"
#include "../common/zmq/zmq_helpers.hpp"
#include "../common/zmq/zmq_message.hpp"

using namespace trustwave;
message_broker::message_broker(zmq::context_t& ctx) :
                context_(ctx), internal_socket_(new zmq::socket_t(context_, ZMQ_ROUTER)), external_socket_(
                                new zmq::socket_t(context_, ZMQ_ROUTER)), workers_(
                                authenticated_scan_server::instance().settings.heartbeat_expiry_), replied_(0)
{
}

//  ---------------------------------------------------------------------
//  Destructor for broker object

message_broker::~message_broker()
{
    std::cerr << replied_ << " client msgs replied" << std::endl;
}

//  ---------------------------------------------------------------------
//  Bind broker to endpoint, can call this multiple times
//  We use a single socket for both clients and workers.

void message_broker::bind_internal()
{
    const auto ep = authenticated_scan_server::instance().settings.broker_worker_listen_ep_;
    internal_socket_->bind(ep);
    AU_LOG_INFO("CDCM broker is internal active at %s", ep.c_str());
}
void message_broker::bind_external()
{
    const auto ep = authenticated_scan_server::instance().settings.broker_client_listen_ep_;
    external_socket_->bind(ep);
    AU_LOG_INFO("CDCM broker is external active at %s", ep.c_str());
}

//  ---------------------------------------------------------------------
//  Delete any idle workers that haven't pinged us in a while.

void message_broker::purge_workers()
{
    std::deque <trustwave::sp_worker_t> toCull;
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto idle_range = workers_.idle_workers();
    for (auto wrk = idle_range.first; wrk != idle_range.second; ++wrk){
        if ((*wrk)->expiry_ <= now)
            toCull.push_back(*wrk);
    }

    for (auto wrk = toCull.begin(); wrk != toCull.end(); ++wrk){
        AU_LOG_DEBUG("deleting expired worker: %s", (*wrk)->identity_.c_str());
        worker_delete(*wrk, 0);
    }
}

//  ---------------------------------------------------------------------
//  Dispatch requests to waiting workers as possible

void message_broker::service_dispatch(std::unique_ptr <zmsg>&& msg, const std::string& id)
{

    /*
     *        get last worker worked on session(id) -> x
     *        if x and x idle
     *          select x
     *        else
     *          select idle worker
     *
     */
    if (msg){                    //  Queue message if any
        requests_.push_back(std::make_pair(std::move(msg), id));
    }
    purge_workers();
    auto p = workers_.idle_workers();
    while (p.first != p.second && !requests_.empty()){
        auto wrk = workers_.get_by_last_worked_session(requests_.front().second);
        if (!wrk || (wrk && !wrk->idle_)){
            AU_LOG_DEBUG("Not found last worked on session %s",id.c_str());
//            auto wrk_it = p.first;
//            auto next = wrk_it;
//            for (++next; next != p.second; ++next){
//                if ((*next)->expiry_ > (*wrk_it)->expiry_)
//                    wrk_it = next;
//            }
//            wrk = *wrk_it;
            wrk = workers_.get_next_worker();
        }
        else
        {
            AU_LOG_DEBUG("Found last session %s",wrk->identity_.c_str());
        }
        auto front_msg = std::move(requests_.front());
        requests_.pop_front();
        worker_send(wrk, MDPW_REQUEST, std::string(""), std::move(front_msg.first));
        workers_.set_busy(wrk->identity_);
        workers_.update_last_worked(wrk->identity_,id);
    }
}

//  ---------------------------------------------------------------------
//  Creates worker if necessary

trustwave::sp_worker_t message_broker::worker_require(std::string identity)
{
    assert(identity.length() != 0);
    //  self->workers is keyed off worker identity
    if (workers_.exists(identity)){
        return workers_.get(identity);
    }
    else{
        auto wrk = std::make_shared <trustwave::worker>(identity);
        workers_.insert(wrk);
        AU_LOG_DEBUG("registering new worker: %s", identity.c_str());
        return wrk;
    }

}

//  ---------------------------------------------------------------------
//  Deletes worker from all data structures, and destroys worker

void message_broker::worker_delete(trustwave::sp_worker_t wrk, bool send_disconnect)
{
    assert(wrk);
    if (send_disconnect){
        worker_send(wrk, MDPW_DISCONNECT, "", NULL);
    }
    workers_.erase(wrk->identity_);
}

//  ---------------------------------------------------------------------
//  Process message sent to us by a worker

void message_broker::worker_process(std::string sender, std::unique_ptr <zmsg>&& msg)
{
    assert(msg && msg->parts() >= 1);     //  At least, command

    std::string command = reinterpret_cast <const char *>(msg->pop_front().c_str());
    bool worker_ready = workers_.exists(sender);
    auto wrk = worker_require(sender);
    if (command.compare(MDPW_READY) == 0){
        if (worker_ready){              //  Not first command in session
            worker_delete(wrk, 1);
        }
        else{
            worker_waiting(wrk);
        }
    }
    else{
        if (command.compare(MDPW_REPLY) == 0){
            if (worker_ready){
                std::cerr<<"got reply"<<std::endl;
                //  Remove & save client return envelope and insert the
                //  protocol header and service name, then rewrap envelope.
                std::string client = msg->unwrap();
                msg->wrap(MDPC_CLIENT, "");
                msg->wrap(client.c_str(), "");
                msg->send(*external_socket_);
                replied_++;
                worker_waiting(wrk);
            }
            else{
                std::cerr<<"got reply Deleting"<<std::endl;
                worker_delete(wrk, 1);
            }
        }
        else{
            if (command.compare(MDPW_HEARTBEAT) == 0){
                if (worker_ready){
                    workers_.update_expiration(sender);
                }
                else{
                    worker_delete(wrk, 1);
                }
            }
            else{
                if (command.compare(MDPW_DISCONNECT) == 0){
                    worker_delete(wrk, 0);
                }
                else{
                    AU_LOG_ERROR("invalid input message (%d)", (int ) *command.c_str());
                }
            }
        }
    }
}

//  ---------------------------------------------------------------------
//  Send message to worker
//  If pointer to message is provided, sends that message

void message_broker::worker_send(trustwave::sp_worker_t worker_ptr, const char *command, std::string option,
                std::unique_ptr <zmsg> _msg)
{
    std::unique_ptr <zmsg> msg(_msg ? new zmsg(*_msg) : new zmsg());

    //  Stack protocol envelope to start of message
    if (option.size() > 0){                 //  Optional frame after command
        msg->push_front(option.c_str());
    }
    msg->push_front(command);
    msg->push_front(MDPW_WORKER);
    //  Stack routing envelope to start of message
    msg->wrap(worker_ptr->identity_.c_str(), "");

    AU_LOG_DEBUG("sending %s to worker", mdps_commands[(int ) *command]);
    AU_LOG_DEBUG("body : %s", msg->body());
    msg->send(*internal_socket_);
}

//  ---------------------------------------------------------------------
//  This worker is now waiting for work

void message_broker::worker_waiting(trustwave::sp_worker_t worker_ptr)
{
    assert(worker_ptr);
    //  Queue to broker and service waiting lists

    workers_.set_idle(worker_ptr->identity_);
    workers_.update_expiration(worker_ptr->identity_);
    // Attempt to process outstanding requests
    service_dispatch(0);
}

//  ---------------------------------------------------------------------
//  Process a request coming from a client

void message_broker::client_process(std::string sender, std::unique_ptr <zmsg> msg)
{
    assert(msg && msg->parts() >= 2);     //  Service name + body

    /*
     * 1. get body
     * 2. parse body
     * 3. for each action
     *    if short
     *        do internal
     *    else

     *        create message , wrap sender and set single action as body
     *        dispatch message
     */
    using namespace tao::json;
    std::string mstr(msg->body());
    const auto t1 = from_string(mstr);
//    //     AU_LOG_DEBUG("msg: %s", to_string(t1, 2).c_str());
//    AU_LOG_DEBUG("msg: %s", to_string(t1, 2).c_str());
    auto a1 = t1.as <trustwave::msg>();
    if (a1.hdr.session_id != std::string("N/A")){
        trustwave::authenticated_scan_server::instance().sessions->touch_by <shared_mem_sessions_cache::id>(
                        a1.hdr.session_id);
    }
    for (auto aa : a1.msgs){
        AU_LOG_DEBUG("Looking for %s", aa->name().c_str());
        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(aa->name());
        if (act1->short_job()){
            auto res = std::make_shared <trustwave::result_msg>();
            trustwave::res_msg res1;
            res1.hdr = a1.hdr;
            res1.msgs.push_back(res);
            if (-1 == act1->act(a1.hdr, aa, res)){
                AU_LOG_DEBUG("action %s returned with an error", aa->name());
            }
            const tao::json::value v1 = res1;
            auto sssss = to_string(v1, 2);
            zmsg* reply = new zmsg;
            reply->body_set(sssss.c_str());
            std::string client = msg->unwrap();
            //      reply->wrap(client.c_str(), "");
            reply->wrap(MDPC_CLIENT, client.c_str());
            reply->wrap(sender.c_str(), "");
            AU_LOG_DEBUG("sending to client :\n %s", msg->to_str().c_str());
            reply->send(*external_socket_);
            replied_++;

        }
        else{
            trustwave::msg tm;
            tm.hdr = a1.hdr;
            tm.msgs.push_back(aa);
            value v(tm);
            auto m = std::make_unique <zmsg>();
            m->body_set(to_string(v, 2).c_str());
            m->wrap(sender.c_str(), "");
            service_dispatch(std::move(m), a1.hdr.session_id);
        }
    }
}

//  Get and process messages forever or until interrupted
void message_broker::broker_loop()
{
    int64_t now = zmq_helpers::clock();
    int64_t heartbeat_at = now + authenticated_scan_server::instance().settings.heartbeat_interval_;
    zmq::pollitem_t items[] = { { internal_socket_->operator void *(), 0, ZMQ_POLLIN, 0 }, {
                    external_socket_->operator void *(), 0, ZMQ_POLLIN, 0 } };
    while (!zmq_helpers::interrupted){
        int64_t timeout = heartbeat_at - now;
        if (timeout < 0)
            timeout = 0;
        zmq::poll(items, 2, static_cast <long>(timeout));
        //  Process next input message, if any
        if (items[0].revents & ZMQ_POLLIN){
            std::unique_ptr <zmsg> msg = std::make_unique <zmsg>(*internal_socket_);
            if (msg->parts() == 0){
                AU_LOG_ERROR("empty message");
            }
            else{
                AU_LOG_DEBUG("received message: %s",msg->to_str().c_str());


                std::string sender = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));
                msg->pop_front(); //empty message
                std::string header = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));

                msg->dump();
                if (header.compare(MDPW_WORKER) == 0){
                    worker_process(sender, std::move(msg));
                }
                else{
                    AU_LOG_ERROR("invalid message: expected MDPW_WORKER");
                }
            }
        }
        if (items[1].revents & ZMQ_POLLIN){
            std::unique_ptr <zmsg> msg = std::make_unique <zmsg>(*external_socket_);
            if (msg->parts() == 0){
                AU_LOG_ERROR("empty message:");
            }
            else{
                AU_LOG_DEBUG("received message: %s",msg->body());


                std::string sender = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));
                msg->pop_front(); //empty message
                std::string header = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));

                if (header.compare(MDPC_CLIENT) == 0){ //fixme assaf should be generic
                    client_process(sender, std::move(msg));
                }

                else{
                    AU_LOG_ERROR("invalid message: expected MDPC_CLIENT got %s", header.c_str());
                }
            }
        }

        //  Disconnect and delete any expired workers
        //  Send heartbeats to idle workers if needed
        now = zmq_helpers::clock();
        if (now >= heartbeat_at){
            purge_workers();
            auto s = workers_.idle_workers();
            for (auto it = s.first; it != s.second; it++){
                worker_send(*it, const_cast <char*>( MDPW_HEARTBEAT), "", NULL);
            }

            heartbeat_at += authenticated_scan_server::instance().settings.heartbeat_interval_;
            now = zmq_helpers::clock();
        }
    }

}

void message_broker::th_func(zmq::context_t &ctx)
{
    message_broker brk(ctx);
    brk.bind_internal();
    brk.bind_external();
    brk.broker_loop();

}
