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
#include "client.hpp"
#include "session.hpp"
#include "protocol/protocol.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "zmq/mdp.hpp"
#include "zmq/zmq_helpers.hpp"
#include "zmq/zmq_message.hpp"
#include <zmq.hpp>
#include <functional>
#include <boost/asio.hpp>
using namespace trustwave;
message_broker::message_broker(zmq::context_t& ctx):
    context_(ctx), internal_socket_(new zmq::socket_t(context_, ZMQ_ROUTER)),
    external_socket_(new zmq::socket_t(context_, ZMQ_ROUTER)),
    workers_(authenticated_scan_server::instance().settings()->heartbeat_expiry_), replied_(0)
{
}

//  ---------------------------------------------------------------------
//  Destructor for broker object

message_broker::~message_broker() {
    AU_LOG_INFO("broker replied %zu client messages",replied_);
    std::cerr <<"broker replied " << replied_ << " client messages" << std::endl;
}

//  ---------------------------------------------------------------------
//  Bind broker to endpoint, can call this multiple times
//  We use a single socket for both clients and workers.

void message_broker::bind_internal()
{
    const auto ep = authenticated_scan_server::instance().settings()->broker_worker_listen_ep_;
    internal_socket_->bind(ep);
    AU_LOG_INFO("CDCM broker is internal active at %s", ep.c_str());
}
void message_broker::bind_external()
{
    const auto ep = authenticated_scan_server::instance().settings()->broker_client_listen_ep_;
    external_socket_->bind(ep);
    AU_LOG_INFO("CDCM broker is external active at %s", ep.c_str());
}

//  ---------------------------------------------------------------------
//  Delete any idle workers that haven't pinged us in a while.

void message_broker::purge_workers()
{
    std::deque<trustwave::sp_worker_t> toCull;
    auto now = std::chrono::system_clock::now();
    auto idle_range = workers_.idle_workers();
    for(auto wrk = idle_range.first; wrk != idle_range.second; ++wrk) {
        auto expiry = (*wrk)->expiry_;
        if(expiry <= now) {
            toCull.push_back(*wrk);
        }
    }

    for(auto& wrk: toCull) {
        AU_LOG_DEBUG("deleting expired worker: %s", wrk->identity_.c_str());
        worker_delete(wrk, false);
    }
}

//  ---------------------------------------------------------------------
//  Dispatch requests to waiting workers as possible

void message_broker::service_dispatch(std::unique_ptr<zmsg>&& msg, const std::string& id)
{
    /*
     *        get last worker worked on session(id) -> x
     *        if x and x idle
     *          select x
     *        else
     *          select idle worker
     *
     */
    if(msg) { //  Queue message if any
        requests_.emplace_back(std::move(msg), id);
    }
    purge_workers();
    auto p = workers_.idle_workers();
    while(p.first != p.second && !requests_.empty()) {
        auto wrk = workers_.get_by_last_worked_session(requests_.front().second);
        if(!wrk || (wrk && !wrk->idle_)) {
            wrk = workers_.get_next_worker();
            AU_LOG_DEBUG1("Not found last worked on session %s using %s", id.c_str(), wrk->identity_.c_str());
        }
        else {
            AU_LOG_DEBUG1("Found last session %s", wrk->identity_.c_str());
        }
        auto front_msg = std::move(requests_.front());
        requests_.pop_front();
        worker_send(wrk, MDPW_REQUEST, std::string(""), std::move(front_msg.first));
        workers_.set_busy(wrk->identity_);
        workers_.update_last_worked(wrk->identity_, id);
    }
}

trustwave::sp_worker_t message_broker::worker_require(const std::string& identity)
{
    assert(identity.length() != 0);
    //  self->workers is keyed off worker identity
    if(workers_.exists(identity)) {
        return workers_.get(identity);
    }
    else {
        auto wrk = std::make_shared<trustwave::worker>(identity);
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
    if(send_disconnect) {
        worker_send(wrk, MDPW_DISCONNECT, "", nullptr);
    }
    workers_.erase(wrk->identity_);
}

//  ---------------------------------------------------------------------
//  Process message sent to us by a worker

void message_broker::worker_process(const std::string& sender, std::unique_ptr<zmsg>&& msg)
{
    assert(msg && msg->parts() >= 1); //  At least, command
    // workers_.dump();
    std::string command = reinterpret_cast<const char*>(msg->pop_front().c_str());
    bool worker_ready = workers_.exists(sender);
    auto wrk = worker_require(sender);
    if(command.compare(MDPW_READY) == 0) {
        if(worker_ready) { //  Not first command in session
            worker_delete(wrk, true);
        }
        else {
            worker_waiting(wrk);
        }
    }
    else {
        if(command.compare(MDPW_REPLY) == 0) {
            if(worker_ready) {
                //  Remove & save client return envelope and insert the
                //  protocol header and service name, then rewrap envelope.
                std::string client = msg->unwrap();
                if(!client.empty()) {
                    msg->wrap(MDPC_CLIENT, "");
                    msg->wrap(client.c_str(), "");
                    msg->send(*external_socket_);
                    replied_++;
                    worker_waiting(wrk);
                }
            }
            else {
                worker_delete(wrk, true);
            }
        }
        else {
            if(command.compare(MDPW_HEARTBEAT) == 0) {
                if(worker_ready) {
                    workers_.update_expiration(sender);
                }
                else {
                    worker_delete(wrk, true);
                }
            }
            else {
                if(command.compare(MDPW_DISCONNECT) == 0) {
                    worker_delete(wrk, false);
                }
                else {
                    AU_LOG_ERROR("invalid input message (%d)", (int)*command.c_str());
                }
            }
        }
    }
    // workers_.dump();
}

//  ---------------------------------------------------------------------
//  Send message to worker
//  If pointer to message is provided, sends that message

void message_broker::worker_send(trustwave::sp_worker_t worker_ptr, const char* command, const std::string& option,
                                 std::unique_ptr<zmsg> _msg)
{
    std::unique_ptr<zmsg> msg(_msg ? new zmsg(*_msg) : new zmsg());

    //  Stack protocol envelope to start of message
    if(!option.empty()) { //  Optional frame after command
        msg->push_front(option.c_str());
    }
    msg->push_front(command);
    msg->push_front(MDPW_WORKER);
    //  Stack routing envelope to start of message
    msg->wrap(worker_ptr->identity_.c_str(), "");

    if(strcmp(MDPW_REQUEST, command) == 0) {
        AU_LOG_DEBUG("sending %s to worker", mdps_commands[(int)*command]);
        AU_LOG_DEBUG("body : %s", msg->body());
    }
    else {
        AU_LOG_DEBUG1("sending %s to worker", mdps_commands[(int)*command]);
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
void message_broker::do_act(trustwave::res_msg& result_message, std::shared_ptr<action_msg> action_message,
                            boost::shared_ptr<session> sess)
{
    AU_LOG_DEBUG1("Looking for %s", action_message->name().c_str());
    auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher().find(action_message->name());
    auto res = std::make_shared<trustwave::result_msg>();
    result_message.msgs.push_back(res);
    if(trustwave::Action_Base::action_status::FAILED == act1->act(sess, action_message, res)) {
        AU_LOG_DEBUG("action %s returned with an error", action_message->name().c_str());
    }
    replied_++;
}
void message_broker::send_local_to_client(const trustwave::res_msg& result_message, const std::string& sender,
                                          const std::string& client)
{
    if(!client.empty() && !sender.empty()) {
        const tao::json::value res_as_json = result_message;
        auto res_body = to_string(res_as_json, 2);
        zmsg* reply = new zmsg;
        reply->body_set(res_body.c_str());
        AU_LOG_DEBUG("sending to client :\n %s", res_body.c_str());
        reply->wrap(MDPC_CLIENT, client.c_str());
        reply->wrap(sender.c_str(), "");
        reply->send(*external_socket_);
    }
}

void message_broker::client_process(const std::string& sender, std::unique_ptr<zmsg>&& msg)
{
    assert(msg && msg->parts() >= 2); //  Service name + body

    /*
     * 1. get body
     * 2. parse body
     * 3. for each action
     *    if found in local dispatcher
     *        do loacally
     *    else
     *        create message , wrap sender and set single action as body
     *        dispatch message
     */
    using namespace tao::json;
    std::string_view mstr(msg->body());
    trustwave::raw_msg unknown_actions_msg;
    trustwave::msg known_actions_msg;
    try {
        const auto req_body_as_json = from_string(mstr);
        auto msg_object = req_body_as_json.get_object();
        known_actions_msg.hdr = unknown_actions_msg.hdr = msg_object.at("H").as<header>();
        auto msgs_array = msg_object.at("msgs").get_array();

        for(auto act_msg: msgs_array) {
            auto action_obj = act_msg.get_object();
            const auto act_key = action_obj.cbegin()->first;
            AU_LOG_DEBUG1("Looking for %s", act_key.c_str());
            auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher().find(act_key);
            if(act1) {
                AU_LOG_DEBUG("%s found", act_key.c_str());
                auto act_m = act1->get_message(act_msg);
                known_actions_msg.msgs.push_back(act_m);
            }
            else {
                AU_LOG_DEBUG("%s not found! perhaps a worker plugin", act_key.c_str());
                unknown_actions_msg.msgs.push_back(action_obj);
            }
        }
    }
    catch(std::exception& e) {
        AU_LOG_ERROR("Malformed message %s", e.what());
        trustwave::res_msg result_message;
        result_message.hdr = unknown_actions_msg.hdr;
        auto sess = trustwave::authenticated_scan_server::instance().get_session(result_message.hdr.session_id);
        auto res = std::make_shared<trustwave::result_msg>();
        result_message.msgs.push_back(res);
        res->id("N/A");
        res->res(std::string("Error: Malformed message ") + e.what()); //error type A
        send_local_to_client(result_message, sender, msg->unwrap());
    }
    AU_LOG_SENSITIVE_DATA_DEBUG("body : %s", msg->body());
    if(unknown_actions_msg.hdr.session_id != std::string("N/A")) {
        trustwave::authenticated_scan_server::instance().sessions->touch_by<shared_mem_sessions_cache::id>(
            unknown_actions_msg.hdr.session_id);
    }
    if(!known_actions_msg.msgs.empty()) {
        trustwave::res_msg result_message;
        result_message.hdr = unknown_actions_msg.hdr;
        auto sess = trustwave::authenticated_scan_server::instance().get_session(result_message.hdr.session_id);
        for(auto action_message: known_actions_msg.msgs) {
            do_act(result_message, action_message, sess);
        }
        send_local_to_client(result_message, sender, msg->unwrap());
    }
    if(!unknown_actions_msg.msgs.empty()) {
        value v(unknown_actions_msg);
        auto m = std::make_unique<zmsg>();
        m->body_set(to_string(v, 2).c_str());
        m->wrap(sender.c_str(), "");
        service_dispatch(std::move(m), unknown_actions_msg.hdr.session_id);
    }
}

void message_broker::handle_message(zmq::socket_t& socket, const std::string& expected_origin,
                                    std::function<void(std::string, std::unique_ptr<zmsg>&&)> process_func)
{
    std::unique_ptr<zmsg> msg = std::make_unique<zmsg>();
    if(!msg->recv(socket) || msg->parts() == 0) {
        AU_LOG_ERROR("empty message");
    }
    else {
        std::string sender = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));
        msg->pop_front(); // empty message
        AU_LOG_DEBUG1("received message from [ %s ]: %s", sender.c_str(), msg->to_str(false, true, false).c_str());
        std::string header = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));
        if(header.compare(expected_origin) == 0) {
            process_func(sender, std::move(msg));
        }
        else {
            AU_LOG_ERROR("invalid message: expected %s", expected_origin.c_str());
        }
    }
}

//  Get and process messages forever or until interrupted
void message_broker::broker_loop()
{
    auto now = zmq_helpers::clock();
    auto heartbeat_at
        = now + std::chrono::milliseconds(authenticated_scan_server::instance().settings()->heartbeat_interval_);
    zmq::pollitem_t items[] = {{internal_socket_->operator void*(), 0, ZMQ_POLLIN, 0},
                               {external_socket_->operator void*(), 0, ZMQ_POLLIN, 0}};
    while(!zmq_helpers::interrupted) {
        auto timeout = heartbeat_at - now;
        if(timeout.count() < 0) timeout.zero();
        zmq::poll(items, 2, timeout.count());
        //  Process next input message, if any
        if(0 != (items[0].revents & ZMQ_POLLIN)) {
            handle_message(
                *internal_socket_, std::string(MDPW_WORKER),
                std::bind(&message_broker::worker_process, this, std::placeholders::_1, std::placeholders::_2));
        }
        if(0 != (items[1].revents & ZMQ_POLLIN)) {
            handle_message(
                *external_socket_, std::string(MDPC_CLIENT),
                std::bind(&message_broker::client_process, this, std::placeholders::_1, std::placeholders::_2));
        }

        //  Disconnect and delete any expired workers
        //  Send heartbeats to idle workers if needed
        now = zmq_helpers::clock();
        if(now >= heartbeat_at) {
            purge_workers();
            auto s = workers_.idle_workers();
            for(auto it = s.first; it != s.second; it++) {
                worker_send(*it, const_cast<char*>(MDPW_HEARTBEAT), "", nullptr);
            }

            heartbeat_at
                += std::chrono::milliseconds(authenticated_scan_server::instance().settings()->heartbeat_interval_);
            now = zmq_helpers::clock();
        }
    }
}

void message_broker::th_func(zmq::context_t& ctx, boost::asio::io_service& ios)
{
    message_broker brk(ctx);
    brk.bind_internal();
    brk.bind_external();
    brk.broker_loop();
    ios.stop();
}
