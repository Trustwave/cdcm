//
// Created by rfrenkel on 8/25/2019.
//

#include "workers_monitor.hpp"
#include <utility>      // std::pair, std::make_pair
#include <boost/filesystem.hpp>
#include <boost/process/extend.hpp>
#include <boost/system/error_code.hpp>
#include <sys/types.h>
#include "../common/singleton_runner/authenticated_scan_server.hpp"
#include "../common/zmq/zmq_helpers.hpp"

namespace bp = boost::process;
using namespace std;
using namespace trustwave;
workers_monitor::workers_monitor(boost::asio::io_service& ios_) : ios(ios_), num_workers (authenticated_scan_server::instance().settings.worker_processes_), worker_bin_path("../backend/cdcm_worker")
{
}

workers_monitor::~workers_monitor()
{
    std::cerr << "worker monitor destructor" << std::endl;
    for (auto iter = workers_pull.begin(); iter != workers_pull.end() ; ++iter )
    {
        std::cerr << "about to terminate worker name: " << iter->first << std::endl;
        iter->second->terminate();
    }
    workers_pull.erase(workers_pull.begin(), workers_pull.end());
}
void workers_monitor::run()
{
    auto num_workers = authenticated_scan_server::instance().settings.worker_processes_;
    for (size_t i=1; i<= num_workers; ++i)
    {
        std::string worker_name = std::to_string(i);
        monitor(worker_name);
    }

}

void workers_monitor::monitor(std::string worker_name)
{
    cout << "in monitor, worker name: " << worker_name << endl;
    auto worker_pair = workers_pull.find(worker_name);
    if (  worker_pair  != workers_pull.end() )
    {
        cout << "worker " << worker_name << " was found in the map" << std::endl;
        auto worker = start_worker(worker_name);
        if (worker != nullptr ) {
            try{
                worker_pair->second.release();
                worker_pair->second = std::move(worker);
            }
            catch (std::exception& exception) {
                std::cerr << "got exception: " << exception.what() << endl;
            }
        }
        else {
            cout << "error: worker process cannot be created" << endl; //ERROR
        }
    }
    else
    {
        cout << "worker " << worker_name << " was NOT found in the map" << std::endl;
        auto worker = start_worker(worker_name);
        if (worker != nullptr ) {
            workers_pull.emplace(worker_name,std::move(worker));
        }
        else {
            cerr  << "error: worker process cannot be created" << endl; //ERROR
        }
    }
}

std::unique_ptr<bp::child> workers_monitor::start_worker(std::string worker_name)
{
    cout << "in start_worker. worker name: " << worker_name << endl;
    try
    {
        auto worker = std::make_unique<bp::child>(worker_bin_path, worker_name,
                  bp::on_exit( [  worker_name, this ](int status, const std::error_code& ec) {
                      std::cout << "on_exit handler called for worker: " << worker_name << std::endl;
                      std::cout << "on_exit status value: " << status << std::endl;
                      std::cout << "on_exit error code value: " << ec.value() << std::endl;
                      std::cout << "on_exit error code value: " << ec.message() << std::endl;
                      std::cout << "on_exit error code category: " << ec.category().name() << std::endl;
                      if(!zmq_helpers::interrupted) {
                          monitor(worker_name);
                      }
                  }),ios);
        return std::move(worker);
    }
    catch (std::exception& exception)
    {
        cout << "got exception: " << exception.what() << endl;
        return nullptr;
    }
}