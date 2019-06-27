//===========================================================================
// Trustwave ltd. @{SRCH}
//								mdclient2.cpp
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

//
//  Majordomo Protocol client example - asynchronous
//  Uses the mdcli API to hide all MDP aspects
//
//  Lets us 'build mdclient' and 'build all'
//
//     Andreas Hoelzlwimmer <andreas.hoelzlwimmer@fh-hagenberg.at>
//
#include "mdcliapi2.hpp"
#include <thread>
#include <boost/uuid/uuid.hpp>         // streaming operators etc.
#include <boost/uuid/random_generator.hpp>         // streaming operators etc.
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include "../misc/protocol/protocol.hpp"
static void fc(int c)
{
    mdcli session("tcp://127.0.0.1:5555", 1);
    auto act_id1 = boost::uuids::random_generator()();
    std::string get_session_m =
                    R"(
          { 
               "H":
               {
                   "session_id" : ")"
                                    + std::string("N/A")
                                    + R"(" 
               },
               "msgs":
                   [

                       {
                           "start_session" :
                           {
                               "id": ")"
                                    + boost::uuids::to_string(act_id1)
                                    + R"(",
                               "remote":"%{host}",
                               "domain":"%{domain}",
                               "username":"%{username}",
                               "password":"%{password}",
                               "workstation":"SSS"
                           }
                       }
                   ]
           }
                   
               
          )";
    zmsg * request1 = new zmsg(get_session_m.c_str());
    session.send("echo", request1);
    delete request1;
    zmsg *reply = session.recv();

    if (reply) {
        std::cout << reply->body() << std::endl;
        using namespace tao::json;
            std::string mstr(reply->body());
            const auto t1 = from_string(mstr);
            //     AU_LOG_DEBUG("msg: %s", to_string(t1, 2).c_str());
            printf("msg: %s", to_string(t1, 2).c_str());
            auto a1 = t1.as<trustwave::res_msg>();

        auto act_id2 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id3 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id4 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id5 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto new_session_id = a1.msgs[0]->res();
        delete reply;
        reply=nullptr;
        std::string name =
                        R"(
          {      
            "H":
                {
                    "session_id" : ")"
                                        + new_session_id
                                        + R"("
                },
            "msgs":
                [
                    {
                            "get_file" :
                            {
                                "id": ")"
                                        + act_id2
                                        + R"(",
                                "param":"ADMIN$/winhlp32.exe"
                            }
                    },
                    {
                            "get_remote_file_version" :
                            {
                                "id": ")"
                                        + act_id3
                                        + R"(",
                                "param":"ADMIN$/winhlp32.exe"
                            }
                    },
                    {
                            "query_value" :
                            {
                                "id": ")"
                                        + act_id4
                                        + R"(",
                                "key":"SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion",
                                "value":"ProductID"
                            }
                    }
                ]
        })";
        printf("%s\n",name.c_str());

        zmsg * request2 = new zmsg(name.c_str());
           session.send("echo", request2);
           delete request2;
           for (int count = 0; count < 3; count++) {
                      zmsg *reply = session.recv ();

                      if (reply) {
                          printf("%s\n",reply->body());
                          delete reply;
                      } else {
                          break;              //  Interrupted by Ctrl-C
                      }
                  }

           //std::string mstr2(reply->body());
//           const auto t2 = from_string(mstr2);
//           const tao::json::value v1 = res2;
               //AU_LOG_INFO("Done %s", to_string(v1, 2).c_str());
//       int count;
//       for (count = 0; count < c; count++) {
//       /*    zmsg * request1 = new zmsg(get_session_m.c_str());
//           session.send ("shrtsss", request1);
//           delete request1;*/
//           zmsg * request2 = new zmsg(get_session_m.c_str());
//                   session.send ("echo", request2);
//                   delete request2;
//
//       }
//       for (count = 0; count < c; count++) {
//           zmsg *reply = session.recv ();
//
//           if (reply) {
//               std::cout<<reply->body()<<std::endl;
//               delete reply;
//           } else {
//               break;              //  Interrupted by Ctrl-C
//           }
//       }
//       std::cout << count << " replies received" << std::endl;
    }
}
int main(int argc, char *argv[])
{
    std::vector<std::thread> tp;
    for (unsigned int i = 0; i < 1; ++i)        //context+broker
    {
        tp.push_back(std::move(std::thread(fc, 1)));

    }
    for (unsigned int i = 0; i < tp.size(); i++) {
        tp.at(i).join();
    }

    return 0;
}
