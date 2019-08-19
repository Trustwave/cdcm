//===========================================================================
// Trustwave ltd. @{SRCH}
//								worker_container.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 18 Jul 2019
// Comments: 

#include "worker_container.hpp"
using namespace trustwave;

int main(int argc, char **argv)
{
    worker_container c(2323);
    auto w=std::make_shared<worker>("XXX",2);
    w->idle_ = true;
    c.insert(w);
    c.is_idle("XXX");

    return 0;
}
