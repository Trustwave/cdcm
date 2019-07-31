//===========================================================================
// Trustwave ltd. @{SRCH}
//								zmsg.hpp
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

#ifndef ZMQ_MESSAGE_HPP_
#define ZMQ_MESSAGE_HPP_

#include <vector>
#include <string>
namespace zmq{
class socket_t;
}
class zmsg
{
public:
    typedef std::basic_string<unsigned char> ustring;

    zmsg();
    //  --------------------------------------------------------------------------
    //  Constructor, sets initial body
    zmsg(char const *body);
    //  -------------------------------------------------------------------------
    //  Constructor, sets initial body and sends message to socket
    zmsg(char const *body, zmq::socket_t &socket);
    //  --------------------------------------------------------------------------
    //  Constructor, calls first receive automatically
    zmsg(zmq::socket_t &socket);
    //  --------------------------------------------------------------------------
    //  Copy Constructor, equivalent to zmsg_dup
    zmsg(zmsg &msg);
    ~zmsg();
    //  --------------------------------------------------------------------------
    //  Erases all messages
    void clear();
    void set_part(size_t part_nbr, unsigned char *data);
    bool recv(zmq::socket_t & socket);
    void send(zmq::socket_t & socket);
    size_t parts();
    void body_set(const char *body);
    void body_fmt(const char *format, ...);
    char * body();
    // zmsg_push
    void push_front(const char *part);
    // zmsg_append
    void push_back(const char *part);

    //  --------------------------------------------------------------------------
    //  Formats 17-byte UUID as 33-char string starting with '@'
    //  Lets us print UUIDs as C strings and use them as addresses
    //
    static char *
    encode_uuid(unsigned char *data);

    // --------------------------------------------------------------------------
    // Formats 17-byte UUID as 33-char string starting with '@'
    // Lets us print UUIDs as C strings and use them as addresses
    //
    static unsigned char *
    decode_uuid(char *uuidstr);
    // zmsg_pop
    ustring pop_front();
    ustring front();
    void append(const char *part);
    char *address();
    void wrap(const char *address, const char *delim);
    std::string unwrap();
    std::string to_str();
    void dump();
private:
    std::vector<ustring> m_part_data;
};

#endif /* ZMQ_MESSAGE_HPP_ */
