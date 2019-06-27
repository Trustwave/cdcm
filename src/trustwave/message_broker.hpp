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

//#include <zmq.hpp>
#include <map>
#include <set>
#include <deque>
#include <string>
#include <memory>

//  We'd normally pull these from config data

#define HEARTBEAT_LIVENESS  3       //  3-5 is reasonable
#define HEARTBEAT_INTERVAL  6000    //  msecs
#define HEARTBEAT_EXPIRY    HEARTBEAT_INTERVAL * HEARTBEAT_LIVENESS
namespace zmq {
class context_t;
class socket_t;
}
class zmsg;
//  This defines a single broker
class message_broker
{
    //  This defines one worker, idle or active
    struct worker
    {
        std::string m_identity;   //  Address of worker
        int64_t m_expiry;         //  Expires at unless heartbeat

        worker(std::string identity, int64_t expiry = 0)
        {
            m_identity = identity;
            m_expiry = expiry;
        }
    };

public:
    static void th_func(zmq::context_t &ctx);    //  ---------------------------------------------------------------------
    //  Destructor for broker object
    ~message_broker();
private:
    //  Get and process messages forever or until interrupted
    void start_brokering();
    //  ---------------------------------------------------------------------
    //  Bind broker to endpoint, can call this multiple times
    //  We use a socket for clients and a socket for  workers.
    void bind_internal(std::string endpoint);
    void bind_external(std::string endpoint);
    //  ---------------------------------------------------------------------
    //  Constructor for broker object
    message_broker(zmq::context_t& ctx);
    //  ---------------------------------------------------------------------
    //  Delete any idle workers that haven't pinged us in a while.
    void purge_workers();
    //  ---------------------------------------------------------------------
    //  Dispatch requests to waiting workers as possible
    void service_dispatch(std::unique_ptr<zmsg>&& msg);
    void service_internal(std::string service_name, std::unique_ptr<zmsg>&& msg);
    //  ---------------------------------------------------------------------
    //  Creates worker if necessary
    worker *
    worker_require(std::string identity);
    //  ---------------------------------------------------------------------
    //  Deletes worker from all data structures, and destroys worker
    void worker_delete(worker *&wrk, int disconnect);
    //  ---------------------------------------------------------------------
    //  Process message sent to us by a worker
    void worker_process(std::string sender, std::unique_ptr<zmsg>&& msg);
    //  ---------------------------------------------------------------------
    //  Send message to worker
    //  If pointer to message is provided, sends that message
    void worker_send(worker *worker, char *command, std::string option, std::unique_ptr<zmsg> _msg);
    //  ---------------------------------------------------------------------
    //  This worker is now waiting for work
    void worker_waiting(worker *worker);
    //  ---------------------------------------------------------------------
    //  Process a request coming from a client
    void client_process(std::string sender, std::unique_ptr<zmsg> msg);

private:
    zmq::context_t& m_context;                  //  0MQ context
    std::unique_ptr<zmq::socket_t> m_internal_socket;           //  Socket for workers
    std::unique_ptr<zmq::socket_t> m_external_socket;           //  Socket for clients
    std::string m_external_endpoint;             //  Broker binds to this endpoint
    std::string m_internal_endpoint;             //  Broker binds to this endpoint
    std::map<std::string, worker*> m_workers;    //  Hash of known workers
    std::set<worker*> m_waiting;                 //  List of waiting workers
    std::deque<std::unique_ptr<zmsg>> m_requests;                //  List of client requests
    size_t replied_;
};

#endif /* MESSAGE_BROKER_HPP_ */
