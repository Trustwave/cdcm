//===========================================================================
// Trustwave ltd. @{SRCH}
//								shared_mem_session.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 16 Jul 2019
// Comments: 
#include "shared_mem_session.hpp"
#include <iostream>
using namespace trustwave;

shared_mem_credentials::shared_mem_credentials(const void_allocator &va) :
                domain_(nullptr, 0, va), username_(nullptr, 0, va), password_(nullptr, 0, va), workstation_(nullptr, 0,
                                va)
{}

std::ostream &trustwave::operator<<(std::ostream &os, const shared_mem_credentials &c)
{
    static const char *indent = "           "; // can also be replaced with 2 tabs (\t)
    os << indent << "domain  : " << c.domain_ << "\n" << indent << "username  : " << c.username_ << "\n" << indent
                    << "workstation  : " << c.workstation_ << "\n";
    return os;
}

shared_mem_session::shared_mem_session(const void_allocator &va) :
                uuid_(nullptr, 0, va), remote_(nullptr, 0, va), creds_(va)
{
}

std::ostream &trustwave::operator<<(std::ostream &os, const shared_mem_session &s)
{
    static const char *indent = "           "; // can also be replaced with 2 tabs (\t)
    os << indent << "ID  : " << s.uuid_ << "\n" << indent << "Remote  : " << s.remote_ << "\n" << indent
                    << "Credentials  : " << s.creds_ << "\n";

    return os;
}

