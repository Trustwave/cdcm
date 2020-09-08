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
    struct cdcm_settings: public configuration {
        static constexpr std::string_view srv_name{"cdcm"};

        cdcm_settings(): configuration(srv_name) {}
        uint32_t                    session_cache_size_         ;
        uint32_t                    session_idle_time_          ;
        uint32_t                    heartbeat_liveness_         ;
        std::chrono::seconds        heartbeat_interval_         ;
        uint32_t                    reconnect_                  ;
        std::chrono::seconds        heartbeat_expiry_           ;
        std::chrono::milliseconds   action_postpone_dur_        ;
        uint32_t                    action_retries_on_postpone_ ;
        uint32_t                    worker_processes_           ;
        std::string                 broker_client_listen_ep_    ;
        std::string                 broker_worker_listen_ep_    ;
        std::string                 worker_connect_ep_          ;
        std::string                 downloaded_files_path_      ;
        std::chrono::seconds        maintenance_interval_       ;
        std::string                 plugins_dir_                ;
        std::string                 cdcm_version_               ;

        std::string to_string() const override
        {
            std::stringstream ss;
            ss <<"\n"
            <<"\t"<<srv_name << " Configuration\n"
            <<"\t"<<"===============================\n"
            <<"\tsession_cache_size          :\t"<< session_cache_size_             <<" MB\n"
            <<"\tsession_idle_time           :\t"<< session_idle_time_              <<" seconds\n"
            <<"\theartbeat_liveness          :\t"<< heartbeat_liveness_             <<"\n"
            <<"\theartbeat_interval          :\t"<< heartbeat_interval_.count()     <<"\n"
            <<"\treconnect                   :\t"<< reconnect_                      <<" milliseconds\n"
            <<"\theartbeat_expiry            :\t"<< heartbeat_expiry_.count()       <<" seconds\n"
            <<"\taction_postpone_dur         :\t"<< action_postpone_dur_.count()    <<" milliseconds\n"
            <<"\taction_retries_on_postpone  :\t"<< action_retries_on_postpone_     <<"\n"
            <<"\tworker_processes            :\t"<< worker_processes_               <<"\n"
            <<"\tbroker_client_listen_ep     :\t"<< broker_client_listen_ep_        <<"\n"
            <<"\tbroker_worker_listen_ep     :\t"<< broker_worker_listen_ep_        <<"\n"
            <<"\tworker_connect_ep           :\t"<< worker_connect_ep_              <<"\n"
            <<"\tdownloaded_files_path       :\t"<< downloaded_files_path_          <<"\n"
            <<"\tmaintenance_interval        :\t"<< maintenance_interval_.count()   <<" seconds\n"
            <<"\tplugins_dir                 :\t"<< plugins_dir_                    <<"\n"
            <<"\tcdcm_version                :\t"<< cdcm_version_                   <<"\n"

            ;
            return ss.str();
        }
    };
} // namespace trustwave

namespace tao::json {
    template<>
    struct traits<trustwave::cdcm_settings>:
        binding::object<
            binding::inherit<traits<trustwave::configuration>>,
            TAO_JSON_BIND_OPTIONAL("session_cache_size", &trustwave::cdcm_settings::session_cache_size_),
            TAO_JSON_BIND_OPTIONAL("session_idle_time", &trustwave::cdcm_settings::session_idle_time_),
            TAO_JSON_BIND_OPTIONAL("heartbeat_liveness", &trustwave::cdcm_settings::heartbeat_liveness_),
            TAO_JSON_BIND_OPTIONAL("heartbeat_interval", &trustwave::cdcm_settings::heartbeat_interval_),
            TAO_JSON_BIND_OPTIONAL("reconnect", &trustwave::cdcm_settings::reconnect_),
            TAO_JSON_BIND_OPTIONAL("heartbeat_expiry", &trustwave::cdcm_settings::heartbeat_expiry_),
            TAO_JSON_BIND_OPTIONAL("action_postpone_dur", &trustwave::cdcm_settings::action_postpone_dur_),
            TAO_JSON_BIND_OPTIONAL("action_retries_on_postpone",
                                   &trustwave::cdcm_settings::action_retries_on_postpone_),
            TAO_JSON_BIND_OPTIONAL("worker_processes", &trustwave::cdcm_settings::worker_processes_),
            TAO_JSON_BIND_OPTIONAL("broker_client_listen_ep", &trustwave::cdcm_settings::broker_client_listen_ep_),
            TAO_JSON_BIND_OPTIONAL("broker_worker_listen_ep", &trustwave::cdcm_settings::broker_worker_listen_ep_),
            TAO_JSON_BIND_OPTIONAL("worker_connect_ep", &trustwave::cdcm_settings::worker_connect_ep_),
            TAO_JSON_BIND_OPTIONAL("downloaded_files_path", &trustwave::cdcm_settings::downloaded_files_path_),
            TAO_JSON_BIND_OPTIONAL("maintenance_interval", &trustwave::cdcm_settings::maintenance_interval_),
            TAO_JSON_BIND_OPTIONAL("plugins_dir", &trustwave::cdcm_settings::plugins_dir_),
    TAO_JSON_BIND_OPTIONAL("cdcm_version", &trustwave::cdcm_settings::cdcm_version_)> {

        template<template<typename...> class Traits>
        [[nodiscard]] static trustwave::cdcm_settings as(const tao::json::basic_value<Traits>& v)
        {
            trustwave::cdcm_settings c;
            const auto o = v.at(trustwave::cdcm_settings::srv_name);
            c.session_cache_size_ = o.template optional<uint32_t>("session_cache_size").value_or(c.session_cache_size_);
            c.session_idle_time_ = o.template optional<uint32_t>("session_idle_time").value_or(c.session_idle_time_);
            c.heartbeat_liveness_ = o.template optional<uint32_t>("heartbeat_liveness").value_or(c.heartbeat_liveness_);
            auto tv = o.template optional<std::chrono::seconds::rep>("heartbeat_interval");
            if(tv) {
                c.heartbeat_interval_ = std::chrono::seconds(tv.value());
            }
            c.reconnect_ = o.template optional<uint32_t>("reconnect").value_or(c.reconnect_);
            tv = o.template optional<std::chrono::seconds::rep>("heartbeat_expiry");
            if(tv) {
                c.heartbeat_expiry_ = std::chrono::seconds(tv.value());
            }
            tv = o.template optional<std::chrono::milliseconds::rep>("action_postpone_dur");
            if(tv) {
                c.action_postpone_dur_ = std::chrono::milliseconds(tv.value());
            }
            c.action_retries_on_postpone_
                = o.template optional<uint32_t>("action_retries_on_postpone").value_or(c.action_retries_on_postpone_);

            c.worker_processes_ = o.template optional<uint32_t>("worker_processes").value_or(c.worker_processes_);
            c.broker_client_listen_ep_
                = o.template optional<std::string>("broker_client_listening_ep").value_or(c.broker_client_listen_ep_);
            c.broker_worker_listen_ep_
                = o.template optional<std::string>("broker_worker_listening_ep").value_or(c.broker_worker_listen_ep_);
            c.worker_connect_ep_ = o.template optional<std::string>("worker_connect_ep").value_or(c.worker_connect_ep_);
            c.downloaded_files_path_
                = o.template optional<std::string>("downloaded_file_path").value_or(c.downloaded_files_path_);
            tv = o.template optional<std::chrono::seconds::rep>("maintenance_interval");
            if(tv) {
                c.maintenance_interval_ = std::chrono::seconds(tv.value());
            }
            c.plugins_dir_ = o.template optional<std::string>("plugins_dir").value_or(c.plugins_dir_);
            c.cdcm_version_ = o.template optional<std::string>("cdcm_version").value_or(c.cdcm_version_);

            return c;
        }
    };
} // namespace tao::json
#endif /* TRUSTWAVE_COMMON_SINGLETON_RUNNER_SETTINGS_HPP_ */
