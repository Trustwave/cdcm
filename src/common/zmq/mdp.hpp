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
namespace trustwave {

    //  This is the version of MDP/Client we implement
    static constexpr const char* MDPC_CLIENT = {"MDPC01"};

    //  This is the version of MDP/Worker we implement
    static constexpr const char* MDPW_WORKER = {"MDPW01"};

    //  MDP/Server commands, as strings
    static constexpr const char* MDPW_READY = {"\001"};
    static constexpr const char* MDPW_REQUEST = {"\002"};
    static constexpr const char* MDPW_REPLY = {"\003"};
    static constexpr const char* MDPW_HEARTBEAT = {"\004"};
    static constexpr const char* MDPW_DISCONNECT = {"\005"};

    static constexpr const char* mdps_commands[] = {nullptr, "READY", "REQUEST", "REPLY", "HEARTBEAT", "DISCONNECT"};

} // namespace trustwave

#endif /* MDP_HPP_ */
