//
// Created by rfrenkel on 8/25/2019.
//

#include "workers_monitor.hpp"
#include <utility> // std::pair, std::make_pair
#include <boost/process/extend.hpp>
#include <boost/system/error_code.hpp>
#include <sys/types.h>
#include "singleton_runner/authenticated_scan_server.hpp"
#include "zmq/zmq_helpers.hpp"

namespace bp = boost::process;
using namespace std;
using namespace trustwave;
workers_monitor::workers_monitor(boost::asio::io_service& ios_):
    ios(ios_), num_workers(authenticated_scan_server::instance().settings()->worker_processes_),
    worker_bin_path("cdcm_worker")
{
}

workers_monitor::~workers_monitor()
{
    // std::cerr << "worker monitor destructor" << std::endl;
    AU_LOG_DEBUG("worker monitor destructor");

    for(auto iter = workers_pull.begin(); iter != workers_pull.end(); ++iter) {
        AU_LOG_DEBUG("about to terminate worker name: %s", iter->first.c_str());
        // std::cerr << "about to terminate worker name: " << iter->first << std::endl;
        iter->second->terminate();
    }
    workers_pull.erase(workers_pull.begin(), workers_pull.end());
}
void workers_monitor::run()
{
    auto num_workers = authenticated_scan_server::instance().settings()->worker_processes_;
    for(size_t i = 1; i <= num_workers; ++i) {
        std::string worker_name = std::to_string(i);
        monitor(worker_name);
    }
}

void workers_monitor::monitor(const std::string& worker_name)
{
    AU_LOG_DEBUG("in monitor, worker name: %s", worker_name.c_str());
    //  cout << "in monitor, worker name: " << worker_name << endl;
    auto worker_pair = workers_pull.find(worker_name);
    if(worker_pair != workers_pull.end()) {
        AU_LOG_DEBUG("worker %s  was found in the map", worker_name.c_str());
        //  cout << "worker " << worker_name << " was found in the map" << std::endl;
        auto worker = start_worker(worker_name);
        if(worker != nullptr) {
            try {
                worker_pair->second.release();
                worker_pair->second = std::move(worker);
            }
            catch(std::exception& exception) {
                AU_LOG_ERROR("got exception while trying to start worker. exception: %s", exception.what());
                //  std::cerr << "got exception: " << exception.what() << endl;
            }
        }
        else {
            AU_LOG_ERROR("worker process cannot be created");
            //  cout << "error: worker process cannot be created" << endl; //ERROR
        }
    }
    else {
        AU_LOG_DEBUG("worker %s  was NOT found in the map", worker_name.c_str());
        //    cout << "worker " << worker_name << " was NOT found in the map" << std::endl;
        auto worker = start_worker(worker_name);
        if(worker != nullptr) {
            workers_pull.emplace(worker_name, std::move(worker));
        }
        //   else {
        //      AU_LOG_ERROR("worker process cannot be created");
        //     cerr  << "error: worker process cannot be created" << endl; //ERROR
        // }
    }
}

std::unique_ptr<bp::child> workers_monitor::start_worker(std::string worker_name)
{
    try {
        auto worker = std::make_unique<bp::child>(bp::search_path(worker_bin_path), worker_name,
                                                  bp::on_exit([worker_name, this](int, const std::error_code&) {
                                                      if(!zmq_helpers::interrupted) {
                                                          monitor(worker_name);
                                                      }
                                                  }),
                                                  ios);
        return std::move(worker);
    }
    catch(std::exception& exception) {
        AU_LOG_ERROR("got exception while trying to start worker. exception: %s", exception.what());
        // cout << "got exception: " << exception.what() << endl;
        return nullptr;
    }
}
