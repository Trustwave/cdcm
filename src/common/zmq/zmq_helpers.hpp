//===========================================================================
// Trustwave ltd. @{SRCH}
//								zhelpers.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 Jun 2019
// Comments: 

#ifndef ZMQ_HELPERS_HPP_
#define ZMQ_HELPERS_HPP_

#include <chrono>  // for time_t
#include <cstdlib>             // for RAND_MAX
#include <string>               // for string
namespace zmq { class socket_t; }
namespace trustwave {
    struct zmq_helpers {
//  Provide random number from 0..(num-1)
#define within(num) (int) ((float) (num) * random () / (RAND_MAX + 1.0))

//  Set simple random printable identity on socket
        static std::string
        set_id(zmq::socket_t &socket);

        static void
        version_assert(int want_major, int want_minor);
//  Return current system clock as milliseconds

        static std::chrono::time_point<std::chrono::system_clock>
        clock();

//  Sleep for a number of milliseconds
        static void
        sleep(std::chrono::milliseconds);

        static void
        console(const char *format, ...);
//  ---------------------------------------------------------------------
//  Signal handling
//
//  Call s_catch_signals() in your application at startup, and then exit
//  your main loop if s_interrupted is ever 1. Works especially well with
//  zmq_poll.

        static bool interrupted;

        static void signal_handler(int);

        static void catch_signals();
    };
}
#endif /* ZMQ_HELPERS_HPP_ */
