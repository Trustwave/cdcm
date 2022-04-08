//===========================================================================
// Trustwave ltd. @{SRCH}
//								shared_mem_tests.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 Jul 2019
// Comments:

#include <boost/smart_ptr/make_shared.hpp>
#include <iostream>
#include <thread>

#include "session.hpp"
#include "sessions_cache/shared_mem_converters.hpp"
#include "sessions_cache/shared_mem_session.hpp"
#include "sessions_cache/shared_mem_sessions_cache.hpp"
using namespace trustwave;

int main(int argc, char** argv)
{
    boost::shared_ptr<trustwave::shared_mem_sessions_cache> sessions;
    std::cerr << "f1\n";
    sessions = trustwave::shared_mem_sessions_cache::get_or_create("sessions", 1024 * 1024 * 10);
    if(!sessions) {
        std::cerr << "Failed alloc sessios" << std::endl;
    }
    else {
        std::cerr << "F1 alloc sessios" << std::endl;
    }
    std::unique_ptr<trustwave::credentials> creds1
        = std::make_unique<trustwave::credentials>("WORKGROUP", "%{username}", "%{password}", "SSS");
    std::vector<boost::shared_ptr<session>> v;
    for(int i = 0; i < 10; ++i) {
        auto e = boost::make_shared<session>(std::string("%{host}") + std::to_string(i), *creds1);
        v.push_back(e);
        sessions->add(e);
        sleep(2);
    }
    // for(auto e:v)
    //{
    //    sessions->remove_by_id(e->idstr());
    //}
    return 0;
}
