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
#include <thread>

#include "../common/client.hpp"
#include "../common/session.hpp"
#include "../common/Logger/include/Logger.h"
#include "../common/protocol/protocol.hpp"
#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/mdp.hpp"
#include "../common/zmq/zmq_helpers.hpp"
#include "../common/zmq/zmq_message.hpp"

using namespace trustwave;
message_worker::message_worker(zmq::context_t &ctx) :
                context_(ctx), heartbeat_at_(), liveness_(
                                authenticated_scan_server::instance().settings.heartbeat_liveness_), heartbeat_(
                                authenticated_scan_server::instance().settings.heartbeat_interval_), reconnect_(
                                authenticated_scan_server::instance().settings.reconnect_), expect_reply_(false), replied_(
                                0)

{
}

//  ---------------------------------------------------------------------
//  Destructor

message_worker::~message_worker()
{
    std::cerr << replied_ << " client msgs replied" << std::endl;
}

//  ---------------------------------------------------------------------
//  Send message to broker
//  If no _msg is provided, creates one internally
void message_worker::send_to_broker(const char *command, std::string option, zmsg *_msg)
{
    zmsg *msg = _msg ? new zmsg(*_msg) : new zmsg();
    //  Stack protocol envelope to start of message
    if (option.length() != 0){
        msg->push_front(option.c_str());
    }
    msg->push_front(command);
    msg->push_front(MDPW_WORKER);
    msg->push_front("");
    std::string cmd = command;
    AU_LOG_DEBUG("I: sending %s to broker", mdps_commands[(int ) *command]);
    if (cmd.compare(MDPW_REPLY) == 0){
        AU_LOG_DEBUG("I: body: %s", msg->body());
    } else{
        AU_LOG_DEBUG1("I: body: %s", msg->body());
    }
    msg->send(*worker_);
    delete msg;
}

//  ---------------------------------------------------------------------
//  Connect or reconnect to broker

void message_worker::connect_to_broker()
{
    worker_.reset(new zmq::socket_t(context_, ZMQ_DEALER));
    int linger = 0;
    worker_->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    zmq_helpers::set_id(*worker_);
    worker_->connect(authenticated_scan_server::instance().settings.worker_connect_ep_);
    AU_LOG_DEBUG("I: connecting to broker at %s...",
                    authenticated_scan_server::instance().settings.worker_connect_ep_.c_str());

    //  Register service with broker
    send_to_broker(MDPW_READY, "", NULL);

    //  If liveness hits zero, queue is considered disconnected
    liveness_ = authenticated_scan_server::instance().settings.heartbeat_liveness_;
    heartbeat_at_ = zmq_helpers::clock() + heartbeat_;
}

//  ---------------------------------------------------------------------
//  Send reply, if any, to broker and wait for next request.

zmsg *
message_worker::recv(zmsg *&reply_p)
{
    //  Format and send the reply if we were provided one
    zmsg *reply = reply_p;
    assert(reply || !expect_reply_);
    if (reply){
        assert(reply_to_.size() != 0);
        reply->wrap(reply_to_.c_str(), "");
        reply_to_ = "";
        send_to_broker(MDPW_REPLY, "", reply);
        ++replied_;
        delete reply_p;
        reply_p = 0;
    }
    expect_reply_ = true;

    while (!zmq_helpers::interrupted){
        zmq::pollitem_t items[] = { { worker_->operator void *(), 0, ZMQ_POLLIN, 0 } };

        zmq::poll(items, 1, heartbeat_.count());

        if (items[0].revents & ZMQ_POLLIN){
            zmsg *msg = new zmsg(*worker_);
            AU_LOG_DEBUG("I: received message from broker body: %s", msg->body());

            liveness_ = authenticated_scan_server::instance().settings.heartbeat_liveness_;

            //  Don't try to handle errors, just assert noisily
            assert(msg->parts() >= 3);

            std::basic_string <unsigned char> empty = msg->pop_front();
            assert(empty.compare(reinterpret_cast <const unsigned char *>("")) == 0);

            std::basic_string <unsigned char> header = msg->pop_front();
            AU_LOG_DEBUG("I: input message (%s)", header.c_str());
            assert(header.compare(reinterpret_cast<const unsigned char * >(MDPW_WORKER)) == 0);

            std::string command = reinterpret_cast <const char *>(msg->pop_front().c_str());
            if (command.compare(MDPW_REQUEST) == 0){
                //  We should pop and save as many addresses as there are
                //  up to a null part, but for now, just save one...
                reply_to_ = msg->unwrap();
                return msg;     //  We have a request to process
            }
            else if (command.compare(MDPW_HEARTBEAT) == 0){
                //  Do nothing for heartbeats
            }
            else if (command.compare(MDPW_DISCONNECT) == 0){
                connect_to_broker();
            }
            else{
                AU_LOG_DEBUG("E: invalid input message (%d)", (int ) *(command.c_str()));
            }
            delete msg;
        }
        else if (--liveness_ == 0){
            AU_LOG_DEBUG("W: disconnected from broker - retrying...");
            //sleep is allowed because there is absolutely nothing to do till reconnect time
            zmq_helpers::sleep(reconnect_);
            connect_to_broker();
        }
        //  Send HEARTBEAT if it's time
        if (zmq_helpers::clock() >= heartbeat_at_){
            send_to_broker( MDPW_HEARTBEAT, "", NULL);
            heartbeat_at_ += heartbeat_;
        }
    }
    if (zmq_helpers::interrupted)
        AU_LOG_DEBUG("W: interrupt received, killing worker...\n");
    return NULL;
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
    zmsg *reply = nullptr;
    while (true){
        zmsg *request = mw.recv(reply);
        if (request == 0){
            break;              //  Worker was interrupted
        }
        std::string mstr(request->body());
        trustwave::msg request_body;
        try {
            const auto req_body_as_json = from_string(mstr);
            AU_LOG_DEBUG("msg: %s", to_string(req_body_as_json, 2).c_str());
            request_body = req_body_as_json.as<trustwave::msg>();
        }
        catch(std::exception& e)
        {
            AU_LOG_ERROR("Malformed message %s",e.what());
            continue;
        }

        trustwave::res_msg res;
        res.hdr = request_body.hdr;

        AU_LOG_DEBUG("actions count is %zu", request_body.msgs.size());
        for (auto action_message : request_body.msgs){
            AU_LOG_DEBUG1("Looking for %s", action_message->name().c_str());

            auto action = trustwave::authenticated_scan_server::instance().public_dispatcher.find(
                            action_message->name());
            if (!action){
                AU_LOG_ERROR("action %s not found", action_message->name().c_str());
            }
            auto result_message = std::make_shared <trustwave::result_msg>();
            result_message->id(action_message->id());
            res.msgs.push_back(result_message);
            if (-1 == action->act(
                                            trustwave::authenticated_scan_server::instance().get_session(
                                                            request_body.hdr.session_id), action_message,
                                            result_message)){
                AU_LOG_DEBUG("action %s returned with an error", action_message->name().c_str());
            }
            AU_LOG_DEBUG("Done %s", res.msgs[0]->res().c_str());
        }
        const tao::json::value v1 = res;
        auto reply_body_str = to_string(v1, 2);
        reply = new zmsg; //will be deleted in recv
        reply->append(reply_body_str.c_str());        //  Echo is complex... :-)
    }
    if (zmq_helpers::interrupted){
        AU_LOG_DEBUG("W: interrupt received, shutting down...\n");
    }
    return 0;
}
