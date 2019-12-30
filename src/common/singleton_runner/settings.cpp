//===========================================================================
// Trustwave ltd. @{SRCH}
//								settings.cpp
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


#include "settings.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;
void cdcm_settings::load(const std::string &filename)
{
    pt::ptree tree;
    pt::read_json(filename, tree);

    heartbeat_liveness_   = tree.get<uint32_t>    ("cdcm.heartbeat_liveness");
    heartbeat_interval_   = std::chrono::seconds(tree.get<uint32_t>    ("cdcm.heartbeat_interval"));
    maintenance_interval_ = std::chrono::seconds(tree.get<uint32_t>    ("cdcm.maintenance_interval"));
    session_idle_time_    = tree.get<uint32_t>    ("cdcm.session_idle_time");
    reconnect_            = tree.get<uint32_t>    ("cdcm.reconnect");
    worker_processes_     = tree.get<uint32_t>    ("cdcm.worker_processes");
    broker_worker_listen_ep_   = tree.get<std::string> ("cdcm.broker_worker_listening_ep");
    broker_client_listen_ep_   = tree.get<std::string> ("cdcm.broker_client_listening_ep");
    worker_connect_ep_  = tree.get<std::string> ("cdcm.worker_connect_ep");
    downloaded_files_path_  = tree.get<std::string> ("cdcm.downloaded_file_path");
    heartbeat_expiry_   = heartbeat_liveness_*heartbeat_interval_;
}
void cdcm_settings::save(const std::string &filename)
{
    pt::ptree tree;
    tree.put("cdcm.heartbeat_liveness"  , heartbeat_liveness_);
    tree.put("cdcm.heartbeat_interval"  , heartbeat_interval_.count());
    tree.put("cdcm.maintenance_interval"  , maintenance_interval_.count());
    tree.put("cdcm.session_idle_time"   , session_idle_time_);
    tree.put("cdcm.reconnect"           , reconnect_);
    tree.put("cdcm.worker_processes"      , worker_processes_);
    tree.put("cdcm.broker_client_listening_ep" , broker_client_listen_ep_);
    tree.put("cdcm.broker_worker_listening_ep" , broker_worker_listen_ep_);
    tree.put("cdcm.worker_connect_ep"   , worker_connect_ep_);
    tree.put("cdcm.downloaded_file_path"   , downloaded_files_path_);
    pt::write_json(filename, tree);
}
