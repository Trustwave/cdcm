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
#ifdef __cplusplus
extern "C" {
#endif
#include "../lib/talloc/talloc.h"
#include "lib/tevent/tevent.h"
#ifdef __cplusplus
}
#endif
using namespace trustwave;
message_broker::message_broker(zmq::context_t& ctx) :
                context_(ctx), internal_socket_(new zmq::socket_t(context_, ZMQ_ROUTER)), external_socket_(
                                new zmq::socket_t(context_, ZMQ_ROUTER)),replied_(0)
{}

//  ---------------------------------------------------------------------
//  Destructor for broker object

message_broker::~message_broker()
{
    std::cerr << replied_ << " client msgs replied" << std::endl;
    while (!workers_.empty()) {
        delete workers_.begin()->second;
        workers_.erase(workers_.begin());
    }
}

//  ---------------------------------------------------------------------
//  Bind broker to endpoint, can call this multiple times
//  We use a single socket for both clients and workers.

void message_broker::bind_internal()
{
    const auto ep = authenticated_scan_server::instance().settings.broker_worker_listen_ep_;
    internal_socket_->bind(ep);
    AU_LOG_DEBUG("I: MDP broker/0.1.1 is internal active at %s", ep.c_str());
}
void message_broker::bind_external()
{
    const auto ep = authenticated_scan_server::instance().settings.broker_client_listen_ep_;
    external_socket_->bind(ep);
    AU_LOG_DEBUG("I: MDP broker/0.1.1 is external active at %s", ep.c_str());
}

//  ---------------------------------------------------------------------
//  Delete any idle workers that haven't pinged us in a while.

void message_broker::purge_workers()
{
    std::deque<worker*> toCull;
    int64_t now = zmq_helpers::clock();
    for (std::set<worker*>::iterator wrk = waiting_.begin(); wrk != waiting_.end(); ++wrk) {
        if ((*wrk)->m_expiry <= now)
            toCull.push_back(*wrk);
    }
    for (std::deque<worker*>::iterator wrk = toCull.begin(); wrk != toCull.end(); ++wrk) {
        AU_LOG_DEBUG("I: deleting expired worker: %s", (*wrk)->m_identity.c_str());
        worker_delete(*wrk, 0);
    }
}

//  ---------------------------------------------------------------------
//  Dispatch requests to waiting workers as possible

void message_broker::service_dispatch(std::unique_ptr<zmsg>&& msg)
{
    if (msg) {                    //  Queue message if any
        requests_.push_back(std::move(msg));
    }

    purge_workers();
    while (!waiting_.empty() && !requests_.empty()) {
        // Choose the most recently seen idle worker; others might be about to expire
        std::set<worker*>::iterator wrk = waiting_.begin();
        std::set<worker*>::iterator next = wrk;
        for (++next; next != waiting_.end(); ++next) {
            if ((*next)->m_expiry > (*wrk)->m_expiry)
                wrk = next;
        }

        auto front_msg = std::move(requests_.front());
        requests_.pop_front();
        worker_send(*wrk, MDPW_REQUEST, std::string(""), std::move(front_msg));
        waiting_.erase(*wrk);
    }
}

void message_broker::service_internal(std::string service_name, std::unique_ptr<zmsg>&& msg)
{
    std::cout << "service_internal " << std::endl;
    msg->dump();
    msg->body_set("ran on internal");
    std::string client = msg->unwrap();
    msg->wrap(MDPC_CLIENT, service_name.c_str());
    msg->wrap(client.c_str(), "");
    msg->send(*external_socket_);
    replied_++;

}

//  ---------------------------------------------------------------------
//  Creates worker if necessary

message_broker::worker *
message_broker::worker_require(std::string identity)
{
    assert(identity.length() != 0);

    //  self->workers is keyed off worker identity
    if (workers_.count(identity)) {
        return workers_.at(identity);
    }
    else {
        worker *wrk = new worker(identity);    //fixme assaf no raw ptrs
        workers_.insert(std::make_pair(identity, wrk));
        AU_LOG_DEBUG("I: registering new worker: %s", identity.c_str());
        return wrk;
    }
}

//  ---------------------------------------------------------------------
//  Deletes worker from all data structures, and destroys worker

void message_broker::worker_delete(worker *&wrk, int disconnect)
{
    assert(wrk);
    if (disconnect) {
        worker_send(wrk, MDPW_DISCONNECT, "", NULL);
    }

    waiting_.erase(wrk);
    //  This implicitly calls the worker destructor
    workers_.erase(wrk->m_identity);
    delete wrk;
}

//  ---------------------------------------------------------------------
//  Process message sent to us by a worker

void message_broker::worker_process(std::string sender, std::unique_ptr<zmsg>&& msg)
{
    assert(msg && msg->parts() >= 1);     //  At least, command

    std::string command = reinterpret_cast<const char *>(msg->pop_front().c_str());
    bool worker_ready = workers_.count(sender) > 0;
    worker *wrk = worker_require(sender);

    if (command.compare(MDPW_READY) == 0) {
        if (worker_ready) {              //  Not first command in session
            worker_delete(wrk, 1);
        }
        else {

            worker_waiting(wrk);
        }
    }
    else {
        if (command.compare(MDPW_REPLY) == 0) {
            if (worker_ready) {
                //  Remove & save client return envelope and insert the
                //  protocol header and service name, then rewrap envelope.
                std::string client = msg->unwrap();
                msg->wrap(MDPC_CLIENT, "");
                msg->wrap(client.c_str(), "");
                msg->send(*external_socket_);
                replied_++;
                worker_waiting(wrk);
            }
            else {
                worker_delete(wrk, 1);
            }
        }
        else {
            if (command.compare(MDPW_HEARTBEAT) == 0) {
                if (worker_ready) {
                    wrk->m_expiry = zmq_helpers::clock()
                                    + authenticated_scan_server::instance().settings.heartbeat_expiry_;
                }
                else {
                    worker_delete(wrk, 1);
                }
            }
            else {
                if (command.compare(MDPW_DISCONNECT) == 0) {
                    worker_delete(wrk, 0);
                }
                else {
                    AU_LOG_DEBUG("E: invalid input message (%d)", (int) *command.c_str());
                    msg->dump();
                }
            }
        }
    }
}

//  ---------------------------------------------------------------------
//  Send message to worker
//  If pointer to message is provided, sends that message

void message_broker::worker_send(worker *worker_ptr, const char *command, std::string option,
                std::unique_ptr<zmsg> _msg)
{
    std::unique_ptr<zmsg> msg(_msg ? new zmsg(*_msg) : new zmsg());

    //  Stack protocol envelope to start of message
    if (option.size() > 0) {                 //  Optional frame after command
        msg->push_front(option.c_str());
    }
    msg->push_front(command);
    msg->push_front(MDPW_WORKER);
    //  Stack routing envelope to start of message
    msg->wrap(worker_ptr->m_identity.c_str(), "");

    AU_LOG_DEBUG("I: sending %s to worker", mdps_commands[(int) *command]);
    msg->dump();
    msg->send(*internal_socket_);
}

//  ---------------------------------------------------------------------
//  This worker is now waiting for work

void message_broker::worker_waiting(worker *worker_ptr)
{
    assert(worker_ptr);
    //  Queue to broker and service waiting lists
    waiting_.insert(worker_ptr);
    worker_ptr->m_expiry = zmq_helpers::clock() + authenticated_scan_server::instance().settings.heartbeat_expiry_;
    // Attempt to process outstanding requests
    service_dispatch(0);
}

//  ---------------------------------------------------------------------
//  Process a request coming from a client

void message_broker::client_process(std::string sender, std::unique_ptr<zmsg> msg)
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
    //     AU_LOG_DEBUG("msg: %s", to_string(t1, 2).c_str());
    printf("msg: %s", to_string(t1, 2).c_str());
    auto a1 = t1.as<trustwave::msg>();
    for (auto aa : a1.msgs) {
        printf("Looking for %s", aa->name().c_str());
        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(aa->name());
        if (act1->short_job()) {
            auto res = std::make_shared<trustwave::result_msg>();
            trustwave::res_msg res1;
            res1.hdr = a1.hdr;
            res1.msgs.push_back(res);
            if (-1 == act1->act(a1.hdr, aa, res)) {
                std::cerr<<aa->name()<<" ERROR"<<std::endl;
                //fixme assaf and????
            }
            const tao::json::value v1 = res1;
            auto sssss = to_string(v1, 2);
            zmsg* reply = new zmsg;
            std::cerr << " XXXXX " << sssss << " XXXXX " << std::endl;
            reply->dump();
            reply->body_set(sssss.c_str());
            std::string client = msg->unwrap();
            //      reply->wrap(client.c_str(), "");
            reply->wrap(MDPC_CLIENT, client.c_str());
            reply->wrap(sender.c_str(), "");
            reply->dump();
            reply->send(*external_socket_);
            replied_++;

        }
        else {
            trustwave::msg tm;
            tm.hdr = a1.hdr;
            tm.msgs.push_back(aa);
            value v(tm);
            auto m = std::make_unique<zmsg>();
            m->body_set(to_string(v, 2).c_str());
            m->wrap(sender.c_str(), "");
            service_dispatch(std::move(m));
        }

    }
}

//  Get and process messages forever or until interrupted
void message_broker::start_brokering()
{
    int64_t now = zmq_helpers::clock();
    int64_t heartbeat_at = now + authenticated_scan_server::instance().settings.heartbeat_interval_;
    zmq::pollitem_t items[] = { { internal_socket_->operator void *(), 0, ZMQ_POLLIN, 0 }, {
                    external_socket_->operator void *(), 0, ZMQ_POLLIN, 0 } };
    while (!zmq_helpers::interrupted) {
        int64_t timeout = heartbeat_at - now;
        if (timeout < 0)
            timeout = 0;
        zmq::poll(items, 2, static_cast<long>(timeout));
        //  Process next input message, if any
        if (items[0].revents & ZMQ_POLLIN) {
            std::unique_ptr<zmsg> msg = std::make_unique<zmsg>(*internal_socket_);
            if (msg->parts() == 0) {
                AU_LOG_DEBUG("E: empty message:");
            }
            else {
                AU_LOG_DEBUG("I: received message:");
                msg->dump();

                std::string sender = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));
                msg->pop_front(); //empty message
                std::string header = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));

                msg->dump();
                if (header.compare(MDPW_WORKER) == 0) {
                    worker_process(sender, std::move(msg));
                }
                else {
                    AU_LOG_DEBUG("E: invalid message: expected MDPW_WORKER");
                    msg->dump();
                }
            }
        }
        if (items[1].revents & ZMQ_POLLIN) {
            std::unique_ptr<zmsg> msg = std::make_unique<zmsg>(*external_socket_);
            if (msg->parts() == 0) {
                AU_LOG_DEBUG("E: empty message:");
            }
            else {

                AU_LOG_DEBUG("I: received message:");
                msg->dump();

                std::string sender = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));
                msg->pop_front(); //empty message
                std::string header = std::string(reinterpret_cast<const char*>(msg->pop_front().c_str()));

                if (header.compare(MDPC_CLIENT) == 0) { //fixme assaf should be generic
                    client_process(sender, std::move(msg));
                }

                else {
                    AU_LOG_DEBUG("E: invalid message: expected MDPC_CLIENT got %s", header.c_str());
                    msg->dump();
                }
            }
        }

        //  Disconnect and delete any expired workers
        //  Send heartbeats to idle workers if needed
        now = zmq_helpers::clock();
        if (now >= heartbeat_at) {
            purge_workers();
            for (std::set<worker*>::iterator it = waiting_.begin(); it != waiting_.end() && (*it) != 0; it++) {
                worker_send(*it, const_cast<char*>( MDPW_HEARTBEAT), "", NULL);
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
    brk.start_brokering();

}
