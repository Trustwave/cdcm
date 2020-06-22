//===========================================================================
// Trustwave ltd. @{SRCH}
//								message_worker.cpp
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
//  Constructor
#include "message_worker.hpp"

#include <zmq.hpp>
#include <iostream>
#include <action.hpp>

#include "client.hpp"
#include "session.hpp"
#include "Logger/include/Logger.h"
#include "protocol/protocol.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "zmq/mdp.hpp"
#include "zmq/zmq_helpers.hpp"
#include "zmq/zmq_message.hpp"

using namespace trustwave;

std::ostream& trustwave::operator<<(std::ostream& os, const trustwave::postponed_action& pa)
{
    return os << pa.id_ << " " << pa.remaining_runs_ << " " << pa.expiration_time_ << std::endl;
}
std::ostream& trustwave::operator<<(std::ostream& os, const trustwave::postponed_actions_queue& q)
{
    os << '\n';
    auto end = q.paq_.end();
    for(auto it = q.paq_.begin(); it != end; ++it) {
        os << *it << '\n';
    }
    return os;
}

message_worker::message_worker(zmq::context_t& ctx):
    context_(ctx), heartbeat_at_(), liveness_(authenticated_scan_server::instance().settings()->heartbeat_liveness_),
    heartbeat_(authenticated_scan_server::instance().settings()->heartbeat_interval_),
    action_postpone_dur_(authenticated_scan_server::instance().settings()->action_postpone_dur_),
    action_retries_on_postpone_(authenticated_scan_server::instance().settings()->action_retries_on_postpone_),
    reconnect_(authenticated_scan_server::instance().settings()->reconnect_), expect_reply_(false), replied_(0)
{
}

//  ---------------------------------------------------------------------
//  Destructor

message_worker::~message_worker() {
    AU_LOG_INFO("worker %s replied %zu client messages",LoggerSource::instance()->get_source_id().c_str(),replied_);
    std::cerr <<"worker "<<LoggerSource::instance()->get_source_id().c_str()<<" replied " << replied_ << " client messages" << std::endl; }

//  ---------------------------------------------------------------------
//  Send message to broker
//  If no _msg is provided, creates one internally
void message_worker::send_to_broker(const char* command, const std::string& option, zmsg* _msg)
{
    zmsg* msg = (nullptr != _msg) ? new zmsg(*_msg) : new zmsg();
    //  Stack protocol envelope to start of message
    if(option.length() != 0) {
        msg->push_front(option.c_str());
    }
    msg->push_front(command);
    msg->push_front(MDPW_WORKER);
    msg->push_front("");
    std::string cmd = command;
    AU_LOG_DEBUG("I: sending %s to broker", mdps_commands[(int)*command]);
    if(cmd.compare(MDPW_REPLY) == 0) {
        AU_LOG_DEBUG("I: body: %s", msg->body());
    }
    else {
        AU_LOG_DEBUG1("I: body: %s", msg->body());
    }
    msg->send(*worker_);
    delete msg;
}

//  ---------------------------------------------------------------------
//  Connect or reconnect to broker

void message_worker::connect_to_broker()
{
    worker_ = std::make_unique<zmq::socket_t>(context_, ZMQ_DEALER);
    int linger = 0;
    worker_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    zmq_helpers::set_id(*worker_);
    worker_->connect(authenticated_scan_server::instance().settings()->worker_connect_ep_);
    AU_LOG_DEBUG("I: connecting to broker at %s...",
                 authenticated_scan_server::instance().settings()->worker_connect_ep_.c_str());

    //  Register service with broker
    send_to_broker(MDPW_READY, "", nullptr);

    //  If liveness hits zero, queue is considered disconnected
    liveness_ = authenticated_scan_server::instance().settings()->heartbeat_liveness_;
    heartbeat_at_ = zmq_helpers::clock() + heartbeat_;
}

//  ---------------------------------------------------------------------
//  Send reply, if any, to broker and wait for next request.
void message_worker::handle_postponed_actions()
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    while(!postponed_actions_.empty() && postponed_actions_.top().expiration_time() <= now) {
        auto top = postponed_actions_.top();
        auto action_message = top.msg();
        auto action_msg_obj = action_message.get_object();
        const auto act_key = action_msg_obj.cbegin()->first;
        AU_LOG_DEBUG("Looking for %s", act_key.c_str());
        auto action = authenticated_scan_server::instance().public_dispatcher().find(act_key);
        if(action) {
            AU_LOG_DEBUG("%s found", act_key.c_str());
            auto act_m = action->get_message(action_message);
            auto action_result = std::make_shared<result_msg>();
            action_result->id(act_m->id());
            auto sess = authenticated_scan_server::instance().get_session(top.get_hdr().session_id);
            auto act_status = action->act(sess, act_m, action_result);
            if(trustwave::Action_Base::action_status::POSTPONED == act_status && top.remaining_runs() > 0) {
                AU_LOG_DEBUG("action %s returned with postponed status updating ", act_key.c_str());
                if(!postponed_actions_.decrement_runs_and_update_expiration(act_m->id())) {
                    AU_LOG_ERROR("action %s doesnt exist ", act_key.c_str());
                }
                //    std::cerr << "Updated "<< postponed_actions_;
            }
            else {
                res_msg result;
                result.hdr = top.get_hdr();
                result.msgs.push_back(action_result);
                const tao::json::value v1 = result;
                auto reply_body_str = to_string(v1);
                auto postponed_reply = std::make_unique<zmsg>(); // will be deleted in recv
                postponed_reply->append(reply_body_str.c_str());
                postponed_reply->wrap(top.reply_to().c_str(), "");
                send_to_broker(MDPW_REPLY, "", postponed_reply.get());

                if(!postponed_actions_.remove_by_id(act_m->id())) {
                    AU_LOG_ERROR("Failed removing action %s -> %s  from postponed queue ", act_key.c_str(),
                                 act_m->id());
                }
                //       std::cerr << "Rermoved "<< postponed_actions_;
                ++replied_;
            }
        }
    }
}
zmsg* message_worker::recv(zmsg*& reply_p)
{
    //  Format and send the reply if we were provided one
    zmsg* reply = reply_p;
    assert(reply || !expect_reply_);
    if(nullptr != reply && !reply_to_.empty()) {
        reply->wrap(reply_to_.c_str(), "");
        reply_to_.clear();
        send_to_broker(MDPW_REPLY, "", reply);
        ++replied_;
        delete reply_p;
        reply_p = nullptr;
    }
    expect_reply_ = true;
    while(!zmq_helpers::interrupted) {
        zmq::pollitem_t items[] = {{worker_->operator void*(), 0, ZMQ_POLLIN, 0}};
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto timeout_ms = heartbeat_.count();
        handle_postponed_actions();
        if(!postponed_actions_.empty() && postponed_actions_.top().expiration_time() > now) {
            timeout_ms = action_postpone_dur_.count();
        }
        zmq::poll(items, 1, timeout_ms);
        if(0 != (items[0].revents & ZMQ_POLLIN)) {
            auto msg = std::make_unique<zmsg>();
            if(msg->recv(*worker_)) {
                AU_LOG_DEBUG("I: received message from broker body: %s", msg->body());
                liveness_ = authenticated_scan_server::instance().settings()->heartbeat_liveness_;
                //  Don't try to handle errors, just assert noisily
                assert(msg->parts() >= 3);
                auto empty = msg->pop_front();
                assert(empty.compare(reinterpret_cast<const unsigned char*>("")) == 0);
                auto header = msg->pop_front();
                AU_LOG_DEBUG("I: input message (%s)", header.c_str());
                assert(header.compare(reinterpret_cast<const unsigned char*>(MDPW_WORKER)) == 0);

                std::string command = reinterpret_cast<const char*>(msg->pop_front().c_str());
                if(command.compare(MDPW_REQUEST) == 0) {
                    //  We should pop and save as many addresses as there are
                    //  up to a null part, but for now, just save one...
                    reply_to_ = msg->unwrap();
                    return msg.release(); //  We have a request to process
                }
                else if(command.compare(MDPW_HEARTBEAT) == 0) {
                    //  Do nothing for heartbeats
                }
                else if(command.compare(MDPW_DISCONNECT) == 0) {
                    connect_to_broker();
                }
                else {
                    AU_LOG_DEBUG("E: invalid input message (%d)", (int)*(command.c_str()));
                }
            }
            else {
                AU_LOG_DEBUG("I: invalid input message from broker body: %s", msg->body());
            }
        }
        else if(--liveness_ == 0) {
            AU_LOG_DEBUG("W: disconnected from broker - retrying...");
            // sleep is allowed because there is absolutely nothing to do till reconnect time
            zmq_helpers::sleep(reconnect_);
            connect_to_broker();
        }
        //  Send HEARTBEAT if it's time
        if(zmq_helpers::clock() >= heartbeat_at_) {
            send_to_broker(MDPW_HEARTBEAT, "", nullptr);
            heartbeat_at_ += heartbeat_;
        }
    }
    if(zmq_helpers::interrupted) {
        AU_LOG_INFO("W: interrupt received, killing worker...\n");
    }
    return nullptr;
}

int message_worker::worker_loop()
{
    AU_LOG_INFO("worker %s starting", LoggerSource::instance()->get_source_id().c_str());
    zmq_helpers::version_assert(4, 0);
    zmq_helpers::catch_signals();
    zmq::context_t ctx(1);
    message_worker mw(ctx);
    mw.connect_to_broker();
    using namespace tao::json;
    zmsg* reply = nullptr;
    while(true) {
        zmsg* request = mw.recv(reply);
        if(request == nullptr) {
            break; //  Worker was interrupted
        }
        std::string mstr(request->body());
        msg request_body;
        res_msg result;
        try {
            const auto req_body_as_json = from_string(mstr);
            AU_LOG_DEBUG("msg: %s", to_string(req_body_as_json, 2).c_str());
            auto msg_object = req_body_as_json.get_object();
            request_body.hdr = msg_object.at("H").as<header>();
            auto msgs_array = msg_object.at("msgs").get_array();
            result.hdr = request_body.hdr;
            AU_LOG_DEBUG("actions count is %zu", msgs_array.size());

            for(auto action_message: msgs_array) {
                auto action_msg_obj = action_message.get_object();
                const auto act_key = action_msg_obj.cbegin()->first;
                AU_LOG_DEBUG("Looking for %s", act_key.c_str());
                auto action_result = std::make_shared<result_msg>();
                auto action = authenticated_scan_server::instance().public_dispatcher().find(act_key);
                if(action) {
                    AU_LOG_DEBUG("%s found", act_key.c_str());
                    auto act_m = action->get_message(action_message);
                    action_result->id(act_m->id());

                    auto sess = authenticated_scan_server::instance().get_session(result.hdr.session_id);
                    auto act_status = action->act(sess, act_m, action_result);

                    if(trustwave::Action_Base::action_status::POSTPONED == act_status) {
                        AU_LOG_DEBUG("action %s returned with postponed status", act_key.c_str());
                        if(!mw.postponed_actions_.add(postponed_action(
                               action_message, mw.reply_to_, act_m->id(), mw.action_retries_on_postpone_,
                               std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()
                                                                    + mw.action_postpone_dur_),
                               result.hdr))) {
                            AU_LOG_ERROR("Failed adding %s to postponed queue", act_key.c_str());
                        }
                        // std::cerr << "Added "<< mw.postponed_actions_;
                    }
                    else {
                        AU_LOG_DEBUG("action %s returned ", act_key.c_str());
                        result.msgs.push_back(action_result);
                    }
                }
                else {
                    AU_LOG_DEBUG("%s not found", act_key.c_str());
                    // try extract id from malformed message
                    try {
                        auto e = action_msg_obj.cbegin()->second.at("id").template as<std::string>();
                        action_result->id(e);
                    }
                    catch(...) {
                        action_result->id("unknown");
                    }
                    action_result->set_resp_code(trustwave::resp_code({2,666}));
                    action_result->res("Error: Malformed message - " + tao::json::to_string(action_message));  //error type A
                    result.msgs.push_back(action_result);
                }
            }
        }
        catch(const std::exception& e) {
            AU_LOG_ERROR("Malformed message %s", e.what());
            auto action_result = std::make_shared<result_msg>();
            action_result->id("unknown");
            action_result->set_resp_code(trustwave::resp_code({2,666}));
            action_result->res("Error: Malformed message");  //error type A
            result.msgs.push_back(action_result);
        }
        try {
            if(!result.msgs.empty()) {
                const tao::json::value v1 = result;
                auto reply_body_str = to_string(v1);
                reply = new zmsg; // will be deleted in recv
                reply->append(reply_body_str.c_str());
            }
            else {
                mw.expect_reply_ = false;
                mw.reply_to_.clear();
            }
        }
        catch(const std::exception& e) {
            AU_LOG_ERROR("Failed building response message %s", e.what());
        }
    }
    if(zmq_helpers::interrupted) {
        AU_LOG_DEBUG("W: interrupt received, shutting down...\n");
    }
    return 0;
}
