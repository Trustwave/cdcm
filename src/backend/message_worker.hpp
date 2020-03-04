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
#include <boost/shared_ptr.hpp>
#include <memory>
#include <chrono>
#include <string>
#include <queue>
#include "utils/postponed_actions.hpp"
namespace zmq {
    class context_t;
    class socket_t;
} // namespace zmq
class zmsg;
//  Structure of our class
//  We access these properties only via class methods
namespace trustwave {
    class session;
    class header;
    class action_msg;
    class res_msg;
    class message_worker final {
    public:
        //  ---------------------------------------------------------------------
        //  Constructor
        explicit message_worker(zmq::context_t& ctx);
        ~message_worker();
        static int worker_loop();
        //  ---------------------------------------------------------------------
        //  Connect or reconnect to broker
        void connect_to_broker();
        //  ---------------------------------------------------------------------
        //  Send reply, if any, to broker and wait for next request.
        zmsg* recv(zmsg*& reply_p);

    private:
        void handle_postponed_actions();
        void send_to_broker(const char* command, const std::string& option, zmsg* _msg);

        zmq::context_t& context_;

        std::unique_ptr<zmq::socket_t> worker_; //  Socket to broker
        //  Heartbeat management
        std::chrono::time_point<std::chrono::system_clock> heartbeat_at_; //  When to send HEARTBEAT
        size_t liveness_; //  How many attempts left
        std::chrono::milliseconds heartbeat_; //  Heartbeat delay, msecs
        std::chrono::milliseconds action_postpone_dur_; //  action postpone, msecs
        u_int32_t action_retries_on_postpone_;
        std::chrono::milliseconds reconnect_; //  Reconnect delay, msecs
        //  Internal state
        bool expect_reply_; //  Zero only at start
        //  Return address, if any
        std::string reply_to_;
        postponed_actions_queue postponed_actions_;
        size_t replied_;
    };
} // namespace trustwave
#endif /* MESSAGE_WORKER_HPP_ */
