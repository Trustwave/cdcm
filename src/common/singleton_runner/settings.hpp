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

#include <cstdint>
#include <string>
#include <chrono>
#include "configuration.hpp"

namespace trustwave {
    struct cdcm_settings : public configuration {
        static constexpr std::string_view srv_name{"cdcm"};

        cdcm_settings() :
                configuration(srv_name) {
        }

        uint32_t session_idle_time_;
        uint32_t heartbeat_liveness_;
        std::chrono::seconds heartbeat_interval_;
        uint32_t reconnect_;
        std::chrono::seconds heartbeat_expiry_;
        uint32_t worker_processes_;
        std::string broker_client_listen_ep_;
        std::string broker_worker_listen_ep_;
        std::string worker_connect_ep_;
        std::string downloaded_files_path_;
        std::chrono::seconds maintenance_interval_;
        std::string plugins_dir_;

    };
}

namespace tao::json {
    template<>
    struct traits<trustwave::cdcm_settings>
            : binding::object<binding::inherit<traits<trustwave::configuration> >,
                    TAO_JSON_BIND_OPTIONAL("session_idle_time", &trustwave::cdcm_settings::session_idle_time_),
                    TAO_JSON_BIND_OPTIONAL("heartbeat_liveness", &trustwave::cdcm_settings::heartbeat_liveness_),
                    TAO_JSON_BIND_OPTIONAL("heartbeat_interval", &trustwave::cdcm_settings::heartbeat_interval_),
                    TAO_JSON_BIND_OPTIONAL("reconnect", &trustwave::cdcm_settings::reconnect_),
                    TAO_JSON_BIND_OPTIONAL("heartbeat_expiry", &trustwave::cdcm_settings::heartbeat_expiry_),
                    TAO_JSON_BIND_OPTIONAL("worker_processes", &trustwave::cdcm_settings::worker_processes_),
                    TAO_JSON_BIND_OPTIONAL("broker_client_listen_ep",
                                           &trustwave::cdcm_settings::broker_client_listen_ep_),
                    TAO_JSON_BIND_OPTIONAL("broker_worker_listen_ep",
                                           &trustwave::cdcm_settings::broker_worker_listen_ep_),
                    TAO_JSON_BIND_OPTIONAL("worker_connect_ep", &trustwave::cdcm_settings::worker_connect_ep_),
                    TAO_JSON_BIND_OPTIONAL("downloaded_files_path", &trustwave::cdcm_settings::downloaded_files_path_),
                    TAO_JSON_BIND_OPTIONAL("maintenance_interval", &trustwave::cdcm_settings::maintenance_interval_),
                    TAO_JSON_BIND_OPTIONAL("plugins_dir", &trustwave::cdcm_settings::plugins_dir_)
            > {
        //TAO_JSON_DEFAULT_KEY(trustwave::smb_service_configuration::srv_name.data());


        template<template<typename...> class Traits>
        [[nodiscard]] static trustwave::cdcm_settings as(const tao::json::basic_value<Traits> &v) {
            trustwave::cdcm_settings c;
            const auto o = v.at(trustwave::cdcm_settings::srv_name);
            c.session_idle_time_ = o.template optional<uint32_t>("session_idle_time").value_or(c.session_idle_time_);
            c.heartbeat_liveness_ = o.template optional<uint32_t>("heartbeat_liveness").value_or(c.heartbeat_liveness_);
            auto tv = o.template optional<std::chrono::seconds::rep>("heartbeat_interval");
            if (tv) {
                c.heartbeat_interval_ = std::chrono::seconds(tv.value());
            }
            c.reconnect_ = o.template optional<uint32_t>("reconnect").value_or(c.reconnect_);
            tv = o.template optional<std::chrono::seconds::rep>("heartbeat_expiry");
            if (tv) {
                c.heartbeat_expiry_ = std::chrono::seconds(tv.value());
            }
            c.worker_processes_ = o.template optional<uint32_t>("worker_processes").value_or(c.worker_processes_);
            c.broker_client_listen_ep_ = o.template optional<std::string>("broker_client_listening_ep").value_or(c.broker_client_listen_ep_);
            c.broker_worker_listen_ep_ = o.template optional<std::string>("broker_worker_listening_ep").value_or(c.broker_worker_listen_ep_);
            c.worker_connect_ep_ = o.template optional<std::string>("worker_connect_ep").value_or(c.worker_connect_ep_);
            c.downloaded_files_path_ = o.template optional<std::string>("downloaded_file_path").value_or(c.downloaded_files_path_);
            tv = o.template optional<std::chrono::seconds::rep>("maintenance_interval");
            if (tv) {
                c.maintenance_interval_ = std::chrono::seconds(tv.value());
            }
            c.plugins_dir_ = o.template optional<std::string>("plugins_dir").value_or(c.plugins_dir_);
return c;
        }
    };
}
#endif /* TRUSTWAVE_COMMON_SINGLETON_RUNNER_SETTINGS_HPP_ */
