//===========================================================================
// Trustwave ltd. @{SRCH}
//								mdp.hpp
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

#ifndef MDP_HPP_
#define MDP_HPP_


//  This is the version of MDP/Client we implement
#define MDPC_CLIENT         "MDPC01"

//  This is the version of MDP/Worker we implement
#define MDPW_WORKER         "MDPW01"

//  MDP/Server commands, as strings
#define MDPW_READY          "\001"
#define MDPW_REQUEST        "\002"
#define MDPW_REPLY          "\003"
#define MDPW_HEARTBEAT      "\004"
#define MDPW_DISCONNECT     "\005"

static char *mdps_commands [] = {
    nullptr, const_cast<char*>("READY"), const_cast<char*>("REQUEST"), const_cast<char*>("REPLY"), const_cast<char*>("HEARTBEAT"), const_cast<char*>("DISCONNECT")
};



#endif /* MDP_HPP_ */
