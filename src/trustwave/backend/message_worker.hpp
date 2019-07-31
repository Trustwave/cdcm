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
    message_worker(zmq::context_t &ctx);
    ~message_worker();
    static int main_func();
    //  ---------------------------------------------------------------------
    //  Connect or reconnect to broker
    void connect_to_broker();
    //  ---------------------------------------------------------------------
    //  Send reply, if any, to broker and wait for next request.
    zmsg *
    recv(zmsg *&reply_p);
private:
    //  ---------------------------------------------------------------------
    //  Destructor
    //  ---------------------------------------------------------------------
    //  Send message to broker
    //  If no _msg is provided, creates one internally
    void send_to_broker(const char *command, std::string option, zmsg *_msg);

    zmq::context_t& context_;
    std::string broker_;
    std::unique_ptr<zmq::socket_t> worker_;     //  Socket to broker
    //  Heartbeat management
    int64_t heartbeat_at_;      //  When to send HEARTBEAT
    size_t liveness_;            //  How many attempts left
    int heartbeat_;              //  Heartbeat delay, msecs
    int reconnect_;              //  Reconnect delay, msecs
    //  Internal state
    bool expect_reply_;           //  Zero only at start
    //  Return address, if any
    std::string reply_to_;
    size_t replied_;
};

#endif /* MESSAGE_WORKER_HPP_ */
