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

#include <cstddef>  // for size_t
#include <memory>    // for unique_ptr
#include <string>    // for string, basic_string
#include <vector>    // for vector

namespace zmq { class socket_t; }
class zmsg final
{
public:
    using ustring = std::basic_string<unsigned char>;

    zmsg()= default;
    explicit zmsg(char const *body);
    zmsg(const zmsg &msg);
    ~zmsg()=default;

    [[nodiscard]] bool recv(zmq::socket_t & socket);
    void send(zmq::socket_t & socket);
    size_t parts();
    void body_set(const char *body);
    char * body();
    void push_front(const char *part);
    ustring& emplace_back(const char *part);

    ustring pop_front();
    void append(const char *part);

    void wrap(const char *address, const char *delim);
    std::string unwrap();
    std::string to_str(bool with_header = true,    bool with_body = true,    bool full = true);
    void dump();
private:
    void clear() noexcept;
    char *address();
    std::vector<ustring> m_part_data;
};

#endif /* ZMQ_MESSAGE_HPP_ */
