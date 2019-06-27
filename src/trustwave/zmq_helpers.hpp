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

#include <string>
#include <stdlib.h>        // random()  RAND_MAX


namespace zmq{
class socket_t;
}
struct zmq_helpers{
//  Provide random number from 0..(num-1)
#define within(num) (int) ((float) (num) * random () / (RAND_MAX + 1.0))

//  Receive 0MQ string from socket and convert into string
static std::string
recv (zmq::socket_t & socket);
//  Convert string to 0MQ string and send to socket
static bool
send (zmq::socket_t & socket, const std::string & string);
//  Sends string as 0MQ string, as multipart non-terminal
static bool
sendmore (zmq::socket_t & socket, const std::string & string);
//  Receives all message parts from socket, prints neatly
//
static void
dump (zmq::socket_t & socket);

//  Set simple random printable identity on socket

static std::string
set_id (zmq::socket_t & socket);
//  Report 0MQ version number
//
static void
version (void);

static void
version_assert (int want_major, int want_minor);
//  Return current system clock as milliseconds

static int64_t
clock (void);

//  Sleep for a number of milliseconds
static void
sleep (int msecs);

static void
console (const char *format, ...);
//  ---------------------------------------------------------------------
//  Signal handling
//
//  Call s_catch_signals() in your application at startup, and then exit
//  your main loop if s_interrupted is ever 1. Works especially well with
//  zmq_poll.

static bool interrupted;
static void signal_handler (int signal_value);
static void catch_signals ();
};

#endif /* ZMQ_HELPERS_HPP_ */
