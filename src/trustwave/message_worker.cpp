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
#include "mdp.hpp"
#include "zmq_message.hpp"
#include "zmq_helpers.hpp"
#include "misc/protocol/protocol.hpp"
#include "authenticated_scan_server.hpp"
#include <zmq.hpp>
#include <iostream>
#include <thread>

message_worker::message_worker(zmq::context_t &ctx, std::string broker) :
                m_context(ctx)
{
    zmq_helpers::version_assert(4, 0);

    m_broker = broker;

    m_worker = nullptr;
    m_expect_reply = false;
    m_heartbeat = 2500;     //  msecs
    m_reconnect = 2500;     //  msecs
    replied_ = 0;

    zmq_helpers::catch_signals();
    connect_to_broker();
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
void message_worker::send_to_broker(char *command, std::string option, zmsg *_msg)
{
    zmsg *msg = _msg ? new zmsg(*_msg) : new zmsg();

    //  Stack protocol envelope to start of message
    if (option.length() != 0) {
        msg->push_front((char*) option.c_str());
    }
    msg->push_front(command);
    msg->push_front((char*) MDPW_WORKER);
    msg->push_front((char*) "");

    zmq_helpers::console("I: sending %s to broker", mdps_commands[(int) *command]);
    msg->dump();
    msg->send(*m_worker);
    delete msg;
}

//  ---------------------------------------------------------------------
//  Connect or reconnect to broker

void message_worker::connect_to_broker()
{
    m_worker.reset(new zmq::socket_t(m_context, ZMQ_DEALER));
    int linger = 0;
    m_worker->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    zmq_helpers::set_id(*m_worker);
    m_worker->connect(m_broker.c_str());
    zmq_helpers::console("I: connecting to broker at %s...", m_broker.c_str());

    //  Register service with broker
    send_to_broker((char*) MDPW_READY, "", NULL);

    //  If liveness hits zero, queue is considered disconnected
    m_liveness = HEARTBEAT_LIVENESS;
    m_heartbeat_at = zmq_helpers::clock() + m_heartbeat;
}

//  ---------------------------------------------------------------------
//  Set heartbeat delay

void message_worker::set_heartbeat(int heartbeat)
{
    m_heartbeat = heartbeat;
}

//  ---------------------------------------------------------------------
//  Set reconnect delay

void message_worker::set_reconnect(int reconnect)
{
    m_reconnect = reconnect;
}

//  ---------------------------------------------------------------------
//  Send reply, if any, to broker and wait for next request.

zmsg *
message_worker::recv(zmsg *&reply_p)
{
    //  Format and send the reply if we were provided one
    zmsg *reply = reply_p;
    assert(reply || !m_expect_reply);
    if (reply) {
        assert(m_reply_to.size() != 0);
        reply->dump();
        reply->wrap(m_reply_to.c_str(), "");
        m_reply_to = "";
        send_to_broker((char*) MDPW_REPLY, "", reply);
        ++replied_;
        delete reply_p;
        reply_p = 0;
    }
    m_expect_reply = true;

    while (!zmq_helpers::interrupted) {
        zmq::pollitem_t items[] = { { m_worker->operator void *(), 0, ZMQ_POLLIN, 0 } };
        zmq::poll(items, 1, m_heartbeat);

        if (items[0].revents & ZMQ_POLLIN) {
            zmsg *msg = new zmsg(*m_worker);
            zmq_helpers::console("I: received message from broker:");
            msg->dump();
            m_liveness = HEARTBEAT_LIVENESS;

            //  Don't try to handle errors, just assert noisily
            assert(msg->parts() >= 3);

            std::basic_string<unsigned char> empty = msg->pop_front();
            assert(empty.compare((unsigned char * )"") == 0);

            std::basic_string<unsigned char> header = msg->pop_front();
            zmq_helpers::console("I: input message (%s)", header.c_str());
            assert(header.compare((unsigned char * )MDPW_WORKER) == 0);

            std::string command = (char*) msg->pop_front().c_str();
            if (command.compare(MDPW_REQUEST) == 0) {
                //  We should pop and save as many addresses as there are
                //  up to a null part, but for now, just save one...
                m_reply_to = msg->unwrap();
                return msg;     //  We have a request to process
            }
            else if (command.compare(MDPW_HEARTBEAT) == 0) {
                //  Do nothing for heartbeats
            }
            else if (command.compare(MDPW_DISCONNECT) == 0) {
                connect_to_broker();
            }
            else {
                zmq_helpers::console("E: invalid input message (%d)", (int) *(command.c_str()));
                msg->dump();
            }
            delete msg;
        }
        else if (--m_liveness == 0) {
            zmq_helpers::console("W: disconnected from broker - retrying...");
            zmq_helpers::sleep(m_reconnect);
            connect_to_broker();
        }
        //  Send HEARTBEAT if it's time
        if (zmq_helpers::clock() >= m_heartbeat_at) {
            send_to_broker((char*) MDPW_HEARTBEAT, "", NULL);
            m_heartbeat_at += m_heartbeat;
        }
    }
    if (zmq_helpers::interrupted)
        printf("W: interrupt received, killing worker...\n");
    return NULL;
}


void message_worker::th_func(zmq::context_t &ctx, const std::string& ep)
    {
    using namespace tao::json;

        message_worker session(ctx, ep);
        zmsg *reply = nullptr;
        while (1) {
            zmsg *request = session.recv(reply);
            if (request == 0) {
                break;              //  Worker was interrupted
            }
            std::string mstr( request->body());
                const auto t1 = from_string(mstr);
               //     AU_LOG_DEBUG("msg: %s", to_string(t1, 2).c_str());
                printf("msg: %s", to_string(t1, 2).c_str());
                    auto a1 = t1.as<trustwave::msg>();
                    auto res = std::make_shared<trustwave::result_msg>();
                    trustwave::res_msg res1;
                    res1.msgs.push_back(res);
                    printf("msgs size is %zu", a1.msgs.size());
                    for (auto aa : a1.msgs) {
                        printf("Looking for %s", aa->name().c_str());
                        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(aa->name());

                        act1->act(a1.hdr, aa, res);
                        //AU_LOG_INFO("Done %s", res1.msgs[0]->res().c_str());
                        printf("Done %s", res1.msgs[0]->res().c_str());
                    }
                    const tao::json::value v1 = res1;
                    reply = new zmsg;
            reply->append(to_string(v1, 2).c_str());        //  Echo is complex... :-)
        }

    }
