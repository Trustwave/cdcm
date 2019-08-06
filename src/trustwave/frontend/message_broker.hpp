//===========================================================================
// Trustwave ltd. @{SRCH}
//								messsage_broker.hpp
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

#ifndef MESSAGE_BROKER_HPP_
#define MESSAGE_BROKER_HPP_

#include "worker_container.hpp"
#include <bits/stdint-intn.h>    // for int64_t
#include <stddef.h>              // for size_t
#include <deque>                 // for deque
#include <memory>                // for unique_ptr
#include <string>                // for string
#include <utility>               // for pair

namespace zmq {
class context_t;
class socket_t;
}
class zmsg;
//  This defines a single broker
namespace trustwave {
class session;
class header;
class message_broker {
    //  This defines one worker, idle or active
    struct worker {
        std::string identity_;   //  Address of worker
        std::string last_worked_session_;   //
        int64_t expiry_;         //  Expires at unless heartbeat

        worker(std::string identity, int64_t expiry = 0)
        {
            identity_ = identity;
            expiry_ = expiry;
        }
    };

public:
    static void th_func(zmq::context_t &ctx);  //  ---------------------------------------------------------------------
    //  Destructor for broker object
    ~message_broker();
private:
    //  Get and process messages forever or until interrupted
    void broker_loop();
    //  ---------------------------------------------------------------------
    //  Bind broker to endpoint, can call this multiple times
    //  We use a socket for clients and a socket for  workers.
    void bind_internal();
    void bind_external();
    //  ---------------------------------------------------------------------
    //  Constructor for broker object
    message_broker(zmq::context_t& ctx);
    //  ---------------------------------------------------------------------
    //  Delete any idle workers that haven't pinged us in a while.
    void purge_workers();
    //  ---------------------------------------------------------------------
    //  Dispatch requests to waiting workers as possible
    void service_dispatch(std::unique_ptr <zmsg>&& msg, const std::string& id = std::string());
    // void service_internal(std::string service_name, std::unique_ptr<zmsg>&& msg);
    //  ---------------------------------------------------------------------
    //  Creates worker if necessary
    trustwave::sp_worker_t
    worker_require(std::string identity);
    //  ---------------------------------------------------------------------
    //  Deletes worker from all data structures, and destroys worker
    void worker_delete(trustwave::sp_worker_t wrk, bool send_disconnect);
    //  ---------------------------------------------------------------------
    //  Process message sent to us by a worker
    void worker_process(std::string sender, std::unique_ptr <zmsg>&& msg);
    //  ---------------------------------------------------------------------
    //  Send message to worker
    //  If pointer to message is provided, sends that message
    void worker_send(trustwave::sp_worker_t worker, const char *command, std::string option,
                    std::unique_ptr <zmsg> _msg);
    //  ---------------------------------------------------------------------
    //  This worker is now waiting for work
    void worker_waiting(trustwave::sp_worker_t worker_ptr);
    //  ---------------------------------------------------------------------
    //  Process a request coming from a client
    void client_process(std::string sender, std::unique_ptr <zmsg> msg);

private:
    zmq::context_t& context_;               //  0MQ context
    std::unique_ptr <zmq::socket_t> internal_socket_;       //  Socket for workers
    std::unique_ptr <zmq::socket_t> external_socket_;       //  Socket for clients
    trustwave::worker_container workers_;
    std::deque <std::pair <std::unique_ptr <zmsg>, std::string>> requests_;              //  List of client requests
    size_t replied_;
};
}
#endif /* MESSAGE_BROKER_HPP_ */
