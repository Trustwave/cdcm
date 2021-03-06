//
// Created by rfrenkel on 8/25/2019.
//

#ifndef TRUSTWAVE_FRONTEND_WORKER_MONITOR_HPP_
#define TRUSTWAVE_FRONTEND_WORKER_MONITOR_HPP_

#include <iostream>
#include <string>
#include <map>
#include <memory>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <boost/process.hpp>
// restore compiler switches
#pragma GCC diagnostic pop
#include <boost/asio.hpp>

class workers_monitor final {
public:
    explicit workers_monitor(boost::asio::io_service& ios);
    ~workers_monitor();
    void run();

private:
    std::unique_ptr<boost::process::child> start_worker(std::string worker_name);
    void monitor(const std::string& worker_name);

private:
    boost::asio::io_service& ios;
    std::map<std::string, std::unique_ptr<boost::process::child>> workers_pool;
    size_t num_workers;
    std::string worker_bin_path;
};

#endif // TRUSTWAVE_FRONTEND_WORKER_MONITOR_HPP_
