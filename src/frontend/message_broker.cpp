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

#include "message_broker.hpp"
#include "../common/client.hpp"
#include "../common/session.hpp"
#include "../common/protocol/protocol.hpp"
#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/mdp.hpp"
#include "../common/zmq/zmq_helpers.hpp"
#include "../common/zmq/zmq_message.hpp"
#include <zmq.hpp>
#include <functional>

using namespace trustwave;
message_broker::message_broker(zmq::context_t &ctx) :
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
    auto now = std::chrono::system_clock::now();
    auto idle_range = workers_.idle_workers();
    for (auto wrk = idle_range.first; wrk != idle_range.second; ++wrk){
        auto expiry=(*wrk)->expiry_;
        if (expiry <= now)
        {
            toCull.push_back(*wrk);

        }
    }

    for (auto & wrk : toCull){
        AU_LOG_DEBUG("deleting expired worker: %s", wrk->identity_.c_str());
        worker_delete(wrk, false);
    }
}

//  ---------------------------------------------------------------------
//  Dispatch requests to waiting workers as possible

void message_broker::service_dispatch(std::unique_ptr <zmsg> &&msg, const std::string &id)
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
        requests_.emplace_back(std::move(msg), id);
    }
    purge_workers();
    auto p = workers_.idle_workers();
    while (p.first != p.second && !requests_.empty()){
        auto wrk = workers_.get_by_last_worked_session(requests_.front().second);
        if (!wrk || (wrk && !wrk->idle_)){
            wrk = workers_.get_next_worker();
            AU_LOG_DEBUG1("Not found last worked on session %s using %s", id.c_str(),wrk->identity_.c_str());
        }
        else{
            AU_LOG_DEBUG1("Found last session %s", wrk->identity_.c_str());
        }
        auto front_msg = std::move(requests_.front());
        requests_.pop_front();
        worker_send(wrk, MDPW_REQUEST, std::string(""), std::move(front_msg.first));
        workers_.set_busy(wrk->identity_);
        workers_.update_last_worked(wrk->identity_, id);
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
        worker_send(wrk, MDPW_DISCONNECT, "", nullptr);
    }
    workers_.erase(wrk->identity_);
}

//  ---------------------------------------------------------------------
//  Process message sent to us by a worker

void message_broker::worker_process(std::string sender, std::unique_ptr <zmsg> &&msg)
{
    assert(msg && msg->parts() >= 1);     //  At least, command
    //workers_.dump();
    std::string command = reinterpret_cast <const char*>(msg->pop_front().c_str());
    bool worker_ready = workers_.exists(sender);
    auto wrk = worker_require(sender);
    if (command.compare(MDPW_READY) == 0){
        if (worker_ready){              //  Not first command in session
            worker_delete(wrk, true);
        }
        else{
            worker_waiting(wrk);
        }
    }
    else{
        if (command.compare(MDPW_REPLY) == 0){
            if (worker_ready){
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
                worker_delete(wrk, true);
            }
        }
        else{
            if (command.compare(MDPW_HEARTBEAT) == 0){
                if (worker_ready){
                    workers_.update_expiration(sender);
                }
                else{
                    worker_delete(wrk, true);
                }
            }
            else{
                if (command.compare(MDPW_DISCONNECT) == 0){
                    worker_delete(wrk, false);
                }
                else{
                    AU_LOG_ERROR("invalid input message (%d)", (int ) *command.c_str());
                }
            }
        }
    }
    //workers_.dump();
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

    if(strcmp(MDPW_REQUEST,command)==0)
    {
        AU_LOG_DEBUG("sending %s to worker", mdps_commands[(int ) *command]);
        AU_LOG_DEBUG("body : %s", msg->body());
    } else
    {
        AU_LOG_DEBUG1("sending %s to worker", mdps_commands[(int ) *command]);
    }
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
    service_dispatch(nullptr);
}

//  ---------------------------------------------------------------------
//  Process a request coming from a client

void message_broker::client_process(std::string sender, std::unique_ptr <zmsg> &&msg)
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
    trustwave::msg recieved_msg;
    try {
        const auto req_body_as_json = from_string(mstr);
        recieved_msg = req_body_as_json.as<trustwave::msg>();
    }
    catch(std::exception& e)
    {
        AU_LOG_ERROR("Malformed message %s",e.what());
        return;
    }
    AU_LOG_DEBUG("body : %s", msg->body());
    if (recieved_msg.hdr.session_id != std::string("N/A")){
        trustwave::authenticated_scan_server::instance().sessions->touch_by <shared_mem_sessions_cache::id>(
                        recieved_msg.hdr.session_id);
    }
    trustwave::msg tm;
    for (auto action_message : recieved_msg.msgs){
        AU_LOG_DEBUG1("Looking for %s", action_message->name().c_str());
        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(action_message->name());
        if(!act1)
        {
            AU_LOG_ERROR("%s not found! ignoring all message", action_message->name().c_str());
            break;
        }
        if (act1->short_job()){
            trustwave::res_msg result_message;
            result_message.hdr = recieved_msg.hdr;
            auto res = std::make_shared <trustwave::result_msg>();
            result_message.msgs.push_back(res);
            if (-1 == act1->act(trustwave::authenticated_scan_server::instance().get_session(
                                                            recieved_msg.hdr.session_id), action_message, res)){
                AU_LOG_DEBUG("action %s returned with an error", action_message->name().c_str());
            }
            const tao::json::value res_as_json = result_message;
            auto res_body = to_string(res_as_json, 2);
            zmsg *reply = new zmsg;
            reply->body_set(res_body.c_str());
            AU_LOG_DEBUG("sending to client :\n %s",res_body.c_str());
            std::string client = msg->unwrap();
            reply->wrap(MDPC_CLIENT, client.c_str());
            reply->wrap(sender.c_str(), "");

            reply->send(*external_socket_);
            replied_++;

        }
        else{
            workers_.dump();

            tm.hdr = recieved_msg.hdr;
            tm.msgs.push_back(action_message);
        }
    }
    if(false == tm.msgs.empty())
    {
        value v(tm);
        auto m = std::make_unique <zmsg>();
        m->body_set(to_string(v, 2).c_str());
        m->wrap(sender.c_str(), "");
        service_dispatch(std::move(m), recieved_msg.hdr.session_id);
    }
}

void message_broker::handle_message(zmq::socket_t &socket, std::string expected_origin,
                std::function <void(std::string, std::unique_ptr <zmsg>&&)> process_func)
{

    std::unique_ptr <zmsg> msg = std::make_unique <zmsg>(socket);
    if (msg->parts() == 0){
        AU_LOG_ERROR("empty message");
    }
    else{

        std::string sender = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));
        msg->pop_front(); //empty message
        AU_LOG_DEBUG1("received message from [ %s ]: %s", sender.c_str(), msg->to_str(false, true, false).c_str());
        std::string header = std::string(reinterpret_cast <const char*>(msg->pop_front().c_str()));
        if (header.compare(expected_origin) == 0){
            process_func(sender, std::move(msg));
        }
        else{
            AU_LOG_ERROR("invalid message: expected %s", expected_origin.c_str());
        }
    }

}

//  Get and process messages forever or until interrupted
void message_broker::broker_loop()
{
    auto now = zmq_helpers::clock();
    auto heartbeat_at = now + std::chrono::milliseconds(authenticated_scan_server::instance().settings.heartbeat_interval_);
    zmq::pollitem_t items[] = { { internal_socket_->operator void *(), 0, ZMQ_POLLIN, 0 }, {
                    external_socket_->operator void *(), 0, ZMQ_POLLIN, 0 } };
    while (!zmq_helpers::interrupted){
        auto timeout = heartbeat_at - now;
        if (timeout.count() < 0)
            timeout.zero();
        zmq::poll(items, 2, static_cast <long>(timeout.count()));
        //  Process next input message, if any
        if (items[0].revents & ZMQ_POLLIN){

            handle_message(*internal_socket_, std::string(MDPW_WORKER),
                            std::bind(&message_broker::worker_process, this, std::placeholders::_1,
                                            std::placeholders::_2));

        }
        if (items[1].revents & ZMQ_POLLIN){

            handle_message(*external_socket_, std::string(MDPC_CLIENT),
                            std::bind(&message_broker::client_process, this, std::placeholders::_1,
                                            std::placeholders::_2));

        }

        //  Disconnect and delete any expired workers
        //  Send heartbeats to idle workers if needed
        now = zmq_helpers::clock();
        if (now >= heartbeat_at){
            purge_workers();
            auto s = workers_.idle_workers();
            for (auto it = s.first; it != s.second; it++){
                worker_send(*it, const_cast <char*>( MDPW_HEARTBEAT), "", nullptr);
            }

            heartbeat_at +=  std::chrono::milliseconds(authenticated_scan_server::instance().settings.heartbeat_interval_);
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
