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

#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <zmq.hpp>
#include "zmq_helpers.hpp"


    zmsg::zmsg() {
    }

   //  --------------------------------------------------------------------------
   //  Constructor, sets initial body
    zmsg::zmsg(char const *body) {
       body_set(body);
   }

   //  -------------------------------------------------------------------------
   //  Constructor, sets initial body and sends message to socket
    zmsg::zmsg(char const *body, zmq::socket_t &socket) {
       body_set(body);
       send(socket);
   }

   //  --------------------------------------------------------------------------
   //  Constructor, calls first receive automatically
    zmsg::zmsg(zmq::socket_t &socket) {
       recv(socket);
   }

   //  --------------------------------------------------------------------------
   //  Copy Constructor, equivalent to zmsg_dup
    zmsg::zmsg(zmsg &msg) {
       m_part_data.resize(msg.m_part_data.size());
       std::copy(msg.m_part_data.begin(), msg.m_part_data.end(), m_part_data.begin());
   }

    zmsg::~zmsg() {
      clear();
   }

   //  --------------------------------------------------------------------------
   //  Erases all messages
   void zmsg::clear() {
       m_part_data.clear();
   }

   void zmsg::set_part(size_t part_nbr, unsigned char *data) {
       if (part_nbr < m_part_data.size()) {
           m_part_data[part_nbr] = data;
       }
   }

   bool zmsg::recv(zmq::socket_t & socket) {
      clear();
      while(1) {
         zmq::message_t message(0);
         try {
            if (!socket.recv(&message, 0)) {
               return false;
            }
         } catch (zmq::error_t& error) {
            std::cout << "E: " << error.what() << std::endl;
            return false;
         }
         //std::cerr << "recv: \"" << (unsigned char*) message.data() << "\", size " << message.size() << std::endl;
         if (message.size() == 17 && ((unsigned char *)message.data())[0] == 0) {
            char *uuidstr = encode_uuid((unsigned char*) message.data());
            push_back(uuidstr);
            delete[] uuidstr;
         }
         else {
            m_part_data.push_back(ustring((unsigned char*) message.data(), message.size()));
         }
         if (!message.more()) {
            break;
         }
      }
      return true;
   }

   void zmsg::send(zmq::socket_t & socket) {
       for (size_t part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++) {
          zmq::message_t message;
          ustring data = m_part_data[part_nbr];
          if (data.size() == 33 && data [0] == '@') {
             unsigned char * uuidbin = decode_uuid (const_cast<char *>(reinterpret_cast<const char*>( data.c_str())));
             message.rebuild(17);
             memcpy(message.data(), uuidbin, 17);
             delete uuidbin;
          }
          else {
             message.rebuild(data.size());
             memcpy(message.data(), data.c_str(), data.size());
          }
          try {
             socket.send(message, part_nbr < m_part_data.size() - 1 ? ZMQ_SNDMORE : 0);
          } catch (zmq::error_t& error) {
             assert(error.num()!=0);
          }
       }
       clear();
   }

   size_t zmsg::parts() {
      return m_part_data.size();
   }

   void zmsg::body_set(const char *body) {
      if (m_part_data.size() > 0) {
         m_part_data.erase(m_part_data.end()-1);
      }
      push_back(body);
   }

   void
   zmsg::body_fmt (const char *format, ...)
   {
       char value [255 + 1];
       va_list args;

       va_start (args, format);
       vsnprintf (value, 255, format, args);
       va_end (args);

       body_set (value);
   }

   char * zmsg::body ()
   {
       if (m_part_data.size())
           return (const_cast<char *>(reinterpret_cast<const char*>(m_part_data [m_part_data.size() - 1].c_str())));
       else
           return nullptr;
   }

   // zmsg_push
   void zmsg::push_front(const char *part) {
      m_part_data.insert(m_part_data.begin(), reinterpret_cast<const unsigned char*>(part));
   }

   // zmsg_append
   void zmsg::push_back(const char *part) {
      m_part_data.push_back(reinterpret_cast<const unsigned char*>(part));
   }

   //  --------------------------------------------------------------------------
   //  Formats 17-byte UUID as 33-char string starting with '@'
   //  Lets us print UUIDs as C strings and use them as addresses
   //
    char *
   zmsg::encode_uuid (unsigned char *data)
   {
       static char
           hex_char [] = "0123456789ABCDEF";

       assert (data [0] == 0);
       char *uuidstr = new char[34];
       uuidstr [0] = '@';
       int byte_nbr;
       for (byte_nbr = 0; byte_nbr < 16; byte_nbr++) {
           uuidstr [byte_nbr * 2 + 1] = hex_char [data [byte_nbr + 1] >> 4];
           uuidstr [byte_nbr * 2 + 2] = hex_char [data [byte_nbr + 1] & 15];
       }
       uuidstr [33] = 0;
       return (uuidstr);
   }


   // --------------------------------------------------------------------------
   // Formats 17-byte UUID as 33-char string starting with '@'
   // Lets us print UUIDs as C strings and use them as addresses
   //
    unsigned char *
   zmsg::decode_uuid (char *uuidstr)
   {
       static char
           hex_to_bin [128] = {
              -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* */
              -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* */
              -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* */
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1, /* 0..9 */
              -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* A..F */
              -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* */
              -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* a..f */
              -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; /* */

       assert (strlen (uuidstr) == 33);
       assert (uuidstr [0] == '@');
       unsigned char *data = new unsigned char[17];
       int byte_nbr;
       data [0] = 0;
       for (byte_nbr = 0; byte_nbr < 16; byte_nbr++)
           data [byte_nbr + 1]
               = (hex_to_bin [uuidstr [byte_nbr * 2 + 1] & 127] << 4)
               + (hex_to_bin [uuidstr [byte_nbr * 2 + 2] & 127]);

       return (data);
   }

   // zmsg_pop
   zmsg::ustring zmsg::pop_front() {
      if (m_part_data.size() == 0) {
         return ustring();
      }
      ustring part = m_part_data.front();
      m_part_data.erase(m_part_data.begin());
      return part;
   }
   zmsg::ustring zmsg::front() {
         if (m_part_data.size() == 0) {
            return ustring();
         }
         ustring part = m_part_data.front();
         //m_part_data.erase(m_part_data.begin());
         return part;
      }
   void zmsg::append (const char *part)
   {
       assert (part);
       push_back(const_cast<char*>(part));
   }

   char *zmsg::address() {
      if (m_part_data.size()>0) {
         return reinterpret_cast<char*>(const_cast<unsigned char *>(m_part_data[0].c_str()));
      } else {
         return 0;
      }
   }

   void zmsg::wrap(const char *address, const char *delim) {
      if (delim) {
         push_front(delim);
      }
      push_front(address);
   }

   std::string zmsg::unwrap() {
      if (m_part_data.size() == 0) {
         return NULL;
      }
      std::string addr = reinterpret_cast<const char*>(pop_front().c_str());
      if (address() && *address() == 0) {
         pop_front();
      }
      return addr;
   }

   void zmsg::dump() {

      std::cerr << "--------------------------------------" << std::endl;
      for (unsigned int part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++) {
          ustring data = m_part_data [part_nbr];

          // Dump the message as text or binary
          int is_text = 1;
          for (unsigned int char_nbr = 0; char_nbr < data.size(); char_nbr++)
              if (data [char_nbr] < 32 || data [char_nbr] > 127)
                  is_text = 0;

          std::cerr << "[" << std::setw(3) << std::setfill('0') << (int) data.size() << "] ";
          for (unsigned int char_nbr = 0; char_nbr < data.size(); char_nbr++) {
              if (is_text) {
                  std::cerr << (char) data [char_nbr];
              } else {
                  std::cerr << std::hex << std::setw(2) << std::setfill('0') << (short int) data [char_nbr];
              }
          }
          std::cerr << std::endl;
      }
   }

   std::string zmsg::to_str() {
       std::stringstream ss;
         ss << "--------------------------------------" << std::endl;
         for (unsigned int part_nbr = 0; part_nbr < m_part_data.size(); part_nbr++) {
             ustring data = m_part_data [part_nbr];

             // Dump the message as text or binary
             int is_text = 1;
             for (unsigned int char_nbr = 0; char_nbr < data.size(); char_nbr++)
                 if (data [char_nbr] < 32 || data [char_nbr] > 127)
                     is_text = 0;

             ss << "[" << std::setw(3) << std::setfill('0') << (int) data.size() << "] ";
             for (unsigned int char_nbr = 0; char_nbr < data.size(); char_nbr++) {
                 if (is_text) {
                     ss << (char) data [char_nbr];
                 } else {
                     ss << std::hex << std::setw(2) << std::setfill('0') << (short int) data [char_nbr];
                 }
             }
             ss << std::endl;
         }
         return ss.str();
      }

