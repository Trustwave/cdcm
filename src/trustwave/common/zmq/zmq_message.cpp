//===========================================================================
// Trustwave ltd. @{SRCH}
//								zmq_message.cpp
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
#include "zmq_message.hpp"
#include <algorithm>           // for copy
#include <cassert>            // for assert
#include <cstdarg>            // for va_end, va_list, va_start
#include <cstdio>             // for vsnprintf
#include <cstring>            // for memcpy, size_t, strlen, NULL
#include <ext/alloc_traits.h>  // for __alloc_traits<>::value_type
#include <functional>          // for function
#include <iomanip>             // for operator<<, setfill, setw
#include <iostream>            // for operator<<, basic_ostream, basic_ostre...
#include <zmq.h>               // for ZMQ_SNDMORE
#include <zmq.hpp>             // for message_t, error_t, socket_t
namespace {
template<class InputIt, class Size, class UnaryFunction>
InputIt for_each_n(InputIt first, Size n, UnaryFunction f)
{
    for (Size i = 0; i < n; ++first, (void) ++i){
        f(*first);
    }
    return first;
}
}

zmsg::zmsg()
= default;

//  --------------------------------------------------------------------------
//  Constructor, sets initial body
zmsg::zmsg(char const *body)
{
    body_set(body);
}

//  -------------------------------------------------------------------------
//  Constructor, sets initial body and sends message to socket
zmsg::zmsg(char const *body, zmq::socket_t &socket)
{
    body_set(body);
    send(socket);
}

//  --------------------------------------------------------------------------
//  Constructor, calls first receive automatically
zmsg::zmsg(zmq::socket_t &socket)
{
    recv(socket);
}

//  --------------------------------------------------------------------------
//  Copy Constructor, equivalent to zmsg_dup
zmsg::zmsg(zmsg &msg)
{
    m_part_data.resize(msg.m_part_data.size());
    std::copy(msg.m_part_data.begin(), msg.m_part_data.end(), m_part_data.begin());
}

zmsg::~zmsg()
{
    clear();
}

//  --------------------------------------------------------------------------
//  Erases all messages
void zmsg::clear()
{
    m_part_data.clear();
}

void zmsg::set_part(size_t part_nbr, unsigned char *data)
{
    if (part_nbr < m_part_data.size()){
        m_part_data[part_nbr] = data;
    }
}

bool zmsg::recv(zmq::socket_t & socket)
{
    clear();
    while (true){
        zmq::message_t message(0);
        try{
            if (!socket.recv(&message, 0)){
                return false;
            }
        } catch (zmq::error_t& error){
            return false;
        }
        if (message.size() == 17 && ((unsigned char *) message.data())[0] == 0){
            auto uuidstr = encode_uuid((unsigned char*) message.data());
            push_back(uuidstr.get());

        }
        else{
            m_part_data.emplace_back((unsigned char*) message.data(), message.size());
        }
        if (!message.more()){
            break;
        }
    }
    return true;
}

void zmsg::send(zmq::socket_t & socket)
{
    for (size_t part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++){
        zmq::message_t message;
        ustring data = m_part_data[part_nbr];
        if (data.size() == 33 && data[0] == '@'){
            auto uuidbin = decode_uuid(const_cast <char *>(reinterpret_cast <const char*>(data.c_str())));
            message.rebuild(17);
            memcpy(message.data(), uuidbin.get(), 17);
        }
        else{
            message.rebuild(data.size());
            memcpy(message.data(), data.c_str(), data.size());
        }
        try{
            socket.send(message, part_nbr < m_part_data.size() - 1 ? ZMQ_SNDMORE : 0);
        } catch (zmq::error_t& error){
            assert(error.num() != 0);
        }
    }
    clear();
}

size_t zmsg::parts()
{
    return m_part_data.size();
}

void zmsg::body_set(const char *body)
{
    if (m_part_data.size() > 0){
        m_part_data.erase(m_part_data.end() - 1);
    }
    push_back(body);
}

void zmsg::body_fmt(const char *format, ...)
{
    char value[255 + 1];
    va_list args;

    va_start(args, format);
    vsnprintf(value, 255, format, args);
    va_end(args);

    body_set(value);
}

char * zmsg::body()
{
    if (m_part_data.size())
        return (const_cast <char *>(reinterpret_cast <const char*>(m_part_data[m_part_data.size() - 1].c_str())));
    else
        return nullptr;
}

// zmsg_push
void zmsg::push_front(const char *part)
{
    m_part_data.insert(m_part_data.begin(), reinterpret_cast <const unsigned char*>(part));
}

// zmsg_append
void zmsg::push_back(const char *part)
{
    m_part_data.emplace_back(reinterpret_cast <const unsigned char*>(part));
}

//  --------------------------------------------------------------------------
//  Formats 17-byte UUID as 33-char string starting with '@'
//  Lets us print UUIDs as C strings and use them as addresses
//
std::unique_ptr <char[]> zmsg::encode_uuid(unsigned char *data)
{
    static const char hex_char[] = "0123456789ABCDEF";

    assert(data[0] == 0);

    auto uuidstr = std::make_unique <char[]>(34);
    uuidstr[0] = '@';
    int byte_nbr;
    for (byte_nbr = 0; byte_nbr < 16; byte_nbr++){
        uuidstr[byte_nbr * 2 + 1] = hex_char[data[byte_nbr + 1] >> 4];
        uuidstr[byte_nbr * 2 + 2] = hex_char[data[byte_nbr + 1] & 15];
    }
    uuidstr[33] = 0;
    return uuidstr;
}

// --------------------------------------------------------------------------
// Formats 17-byte UUID as 33-char string starting with '@'
// Lets us print UUIDs as C strings and use them as addresses
//
std::unique_ptr <char[]> zmsg::decode_uuid(char *uuidstr)
{
    static const char hex_to_bin[128] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* */
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, /* 0..9 */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* A..F */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* a..f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }; /* */

    assert(strlen(uuidstr) == 33);
    assert(uuidstr[0] == '@');
    auto data = std::make_unique <char[]>(17);
    int byte_nbr;
    data[0] = 0;
    for (byte_nbr = 0; byte_nbr < 16; byte_nbr++)
        data[byte_nbr + 1] = (hex_to_bin[uuidstr[byte_nbr * 2 + 1] & 127] << 4)
                        + (hex_to_bin[uuidstr[byte_nbr * 2 + 2] & 127]);
    return data;
}

// zmsg_pop
zmsg::ustring zmsg::pop_front()
{
    if (m_part_data.size() == 0){
        return ustring();
    }
    ustring part = m_part_data.front();
    m_part_data.erase(m_part_data.begin());
    return part;
}
zmsg::ustring zmsg::front()
{
    if (m_part_data.size() == 0){
        return ustring();
    }
    ustring part = m_part_data.front();
    //m_part_data.erase(m_part_data.begin());
    return part;
}
void zmsg::append(const char *part)
{
    assert(part);
    push_back(const_cast <char*>(part));
}

char *zmsg::address()
{
    if (m_part_data.size() > 0){
        return reinterpret_cast <char*>(const_cast <unsigned char *>(m_part_data[0].c_str()));
    }
    else{
        return 0;
    }
}

void zmsg::wrap(const char *address, const char *delim)
{
    if (delim){
        push_front(delim);
    }
    push_front(address);
}

std::string zmsg::unwrap()
{
    if (m_part_data.size() == 0){
        return NULL;
    }
    std::string addr = reinterpret_cast <const char*>(pop_front().c_str());
    if (address() && *address() == 0){
        pop_front();
    }
    return addr;
}

void zmsg::dump()
{

    std::cerr << "--------------------------------------" << std::endl;
    for (auto data : m_part_data){
        // Dump the message as text or binary
        int is_text = 1;
        for (unsigned char char_nbr : data)
            if (char_nbr < 32 || char_nbr > 127)
                is_text = 0;

        std::cerr << "[" << std::setw(3) << std::setfill('0') << data.size() << "] ";
        for (unsigned char char_nbr : data){
            if (is_text){
                std::cerr << (char) char_nbr;
            }
            else{
                std::cerr << std::hex << std::setw(2) << std::setfill(' ') << (unsigned short int) char_nbr;
            }
        }
        std::cerr << std::endl;
    }
}

std::string zmsg::to_str(bool with_header, bool with_body, bool full)
{
    if (!with_header && !with_body && !full){
        return std::string();
    }
    auto check_is_text = [](const ustring& data)->bool
    {
        for (unsigned char char_nbr : data)
        if (char_nbr < 32 || char_nbr > 127){
            return false;
        }
        return true;
    };
    std::stringstream ss;
    using printer = std::function<void(unsigned char c)>;
    printer text_print = [&](unsigned char c)->void
    {
        ss << (char)c;
    };

    printer binary_print = [&](unsigned char c)->void
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (short int) c;
    };
    auto part_print = [&](const ustring& data)
    {
        bool is_text = check_is_text(data);
        ss << "[" << std::setw(3) << std::setfill('0') << (int) data.size() << "] ";
        ::for_each_n(data.begin(), full?data.size():30, is_text ? text_print : binary_print);
        ss << std::endl;
    };
    if (with_header){
        ::for_each_n(m_part_data.begin(), m_part_data.size() - 1, part_print);
    }
    if (with_body){
        part_print(m_part_data[m_part_data.size() - 1]);
    }

    return ss.str();
}

