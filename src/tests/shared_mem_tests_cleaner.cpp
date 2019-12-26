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

#include <iostream>
#include <thread>

#include "session.hpp"
#include "sessions_cache/shared_mem_converters.hpp"
#include "sessions_cache/shared_mem_session.hpp"
#include "sessions_cache/shared_mem_sessions_cache.hpp"
using namespace trustwave;



int main(int argc, char **argv)
{
    boost::shared_ptr<trustwave::shared_mem_sessions_cache> sessions;
        sessions = trustwave::shared_mem_sessions_cache::get_or_create("sessions",1024*1024*10);
        if(!sessions)
            {
                std::cerr<<"Failed alloc sessios"<<std::endl;

            }
        for(int i = 0 ; i < 40 ; ++i){
        sleep(3);
        sessions->dump_by_expiration();
        sessions->clean();
        sessions->dump_by_expiration();
        }

    return 0;
}

