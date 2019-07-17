//===========================================================================
// Trustwave ltd. @{SRCH}
//								settings.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 3 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_COMMON_SINGLETON_RUNNER_SETTINGS_HPP_
#define TRUSTWAVE_COMMON_SINGLETON_RUNNER_SETTINGS_HPP_
#include <string>
#include <stdint.h>

struct cdcm_settings
{

    uint32_t session_idle_time_;
    uint32_t heartbeat_liveness_;
    uint32_t heartbeat_interval_;
    uint32_t reconnect_;
    uint32_t heartbeat_expiry_;
    uint32_t worker_threads_;
    std::string broker_client_listen_ep_;
    std::string broker_worker_listen_ep_;
    std::string worker_connect_ep_;
    void load(const std::string &filename);
    void save(const std::string &filename);
};
#endif /* TRUSTWAVE_COMMON_SINGLETON_RUNNER_SETTINGS_HPP_ */
