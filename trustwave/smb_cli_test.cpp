/*
 * reg_cli_test.cpp
 *
 *  Created on: Apr 17, 2019
 *      Author: root
 */
//===========================================================================
//                              Include Files.
//===========================================================================
//#include "misc/Logger/include/Logger.h"

#include <iostream>
#include <string>
#include "misc/session.hpp"
#include "misc/protocol/protocol.hpp"
#include "authenticated_scan_server.hpp"
#include <boost/uuid/uuid.hpp>         // streaming operators etc.
#include <boost/uuid/random_generator.hpp>         // streaming operators etc.
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <memory>

#include "misc/action.hpp"
using namespace tao::json;
void session_cont_test()
{
    trustwave::credentials creds1("domain1", "username1", "password1", "workstation1");
    trustwave::session s1("remote1", creds1);
    trustwave::authenticated_scan_server::instance().sessions.insert_session(s1);

    trustwave::authenticated_scan_server::instance().sessions.dump_by_time();

    sleep(2);

    trustwave::credentials creds2("domain2", "username2", "password2", "workstation2");
    trustwave::session s2("remote2", creds2);
    trustwave::authenticated_scan_server::instance().sessions.insert_session(s2);
    trustwave::authenticated_scan_server::instance().sessions.dump_by_time();
    sleep(2);
    trustwave::authenticated_scan_server::instance().sessions.get_session_by_dest("remote1");
    trustwave::authenticated_scan_server::instance().sessions.dump_by_time();
    sleep(2);
    trustwave::credentials creds3("domain3", "username3", "password3", "workstation3");
    trustwave::session s3("remote3", creds3);
    trustwave::authenticated_scan_server::instance().sessions.insert_session(s3);
    sleep(2);
    trustwave::authenticated_scan_server::instance().sessions.get_session_by_dest("remote1");
    trustwave::authenticated_scan_server::instance().sessions.dump_by_time();
}

int main(int argc, const char **argv)
{
    AU_LOG_INFO("HI");
    AU_LOG_ERROR("HI");
    AU_LOG_WARNING("HI");

    //session_cont_test();
    //return 0;
    auto act_id1 = boost::uuids::random_generator()();
    std::string get_session_m = R"(
       { 
            "H":
            {
                "session_id" : ")" + std::string("N/A") + R"(" 
            },
            "msgs":
                [

                    {
                        "start_session" :
                        {
                            "id": ")" + boost::uuids::to_string(act_id1) + R"(",
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

    const auto t1 = from_string(get_session_m);
    AU_LOG_DEBUG("msg: %s",to_string(t1,2).c_str());
    auto a1 = t1.as<trustwave::msg>();
    auto res = std::make_shared<trustwave::result_msg>();
    trustwave::res_msg res1;
    res1.msgs.push_back(res);
    for (auto aa : a1.msgs) {
        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(aa->name());

        act1->act(a1.hdr, aa, res);
        AU_LOG_INFO("Done %s",res1.msgs[0]->res().c_str());
    }

    auto act_id2 = boost::uuids::to_string(boost::uuids::random_generator()());
    auto act_id3 = boost::uuids::to_string(boost::uuids::random_generator()());
    auto act_id4 = boost::uuids::to_string(boost::uuids::random_generator()());
    auto act_id5 = boost::uuids::to_string(boost::uuids::random_generator()());
    auto new_session_id = res1.msgs[0]->res();
    std::string name = R"(
      {      
        "H":
            {
                "session_id" : ")" + new_session_id + R"("
            },
        "msgs":
            [
                {
                        "get_file" :
                        {
                            "id": ")" + act_id2 + R"(",
                            "param":"ADMIN$/winhlp32.exe"
                        }
                },
                {
                        "get_remote_file_version" :
                        {
                            "id": ")" + act_id3 + R"(",
                            "param":"ADMIN$/winhlp32.exe"
                        }
                },
                {
                        "query_value" :
                        {
                            "id": ")" + act_id4 + R"(",
                            "key":"SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion",
                            "value":"ProductID"
                        }
                }
            ]
    })";
    const auto t = from_string(name);
    AU_LOG_DEBUG("msg: %s",to_string(t,2).c_str());
    auto a = t.as<trustwave::msg>();
    trustwave::res_msg res2;
    res2.hdr=a.hdr;

    for (auto aa : a.msgs) {
        auto act1 = trustwave::authenticated_scan_server::instance().public_dispatcher.find(aa->name());
        auto res3 = std::make_shared<trustwave::result_msg>();
           res2.msgs.push_back(res3);
        act1->act(a.hdr, aa, res3);
        AU_LOG_INFO("Done %s",res2.msgs[0]->res().c_str());
    }
    const tao::json::value v1 = res2;
    AU_LOG_INFO("Done %s",to_string(v1,2).c_str());
    return 0;
}
