//===========================================================================
// Trustwave ltd. @{SRCH}
//								worker.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 25 Jun 2019
// Comments: 

#ifndef MESSAGE_WORKER_HPP_
#define MESSAGE_WORKER_HPP_
#include <memory>
#include <string>
namespace zmq {
class context_t;
class socket_t;
}
class zmsg;
//  Structure of our class
//  We access these properties only via class methods
class message_worker
{
public:

    //  ---------------------------------------------------------------------
    //  Constructor
    message_worker(zmq::context_t &ctx, std::string broker);
    ~message_worker();
    void th_func();
private:
    //  ---------------------------------------------------------------------
    //  Destructor
    //  ---------------------------------------------------------------------
    //  Send message to broker
    //  If no _msg is provided, creates one internally
    void send_to_broker(const char *command, std::string option, zmsg *_msg);
    //  ---------------------------------------------------------------------
    //  Connect or reconnect to broker
    void connect_to_broker();
    //  ---------------------------------------------------------------------
    //  Set heartbeat delay
    void set_heartbeat(int heartbeat);
    //  ---------------------------------------------------------------------
    //  Set reconnect delay
    void set_reconnect(int reconnect);
    //  ---------------------------------------------------------------------
    //  Send reply, if any, to broker and wait for next request.
    zmsg *
    recv(zmsg *&reply_p);

    std::string m_broker;
    zmq::context_t& m_context;
    std::unique_ptr<zmq::socket_t> m_worker;     //  Socket to broker
    //  Heartbeat management
    int64_t m_heartbeat_at;      //  When to send HEARTBEAT
    size_t m_liveness;            //  How many attempts left
    int m_heartbeat;              //  Heartbeat delay, msecs
    int m_reconnect;              //  Reconnect delay, msecs
    //  Internal state
    bool m_expect_reply;           //  Zero only at start
    //  Return address, if any
    std::string m_reply_to;
    size_t replied_;
};

#endif /* MESSAGE_WORKER_HPP_ */
