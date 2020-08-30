//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														clients_deleter.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/26/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_SINGLETON_RUNNER_CLIENTS_DELETER_HPP
#define SRC_COMMON_SINGLETON_RUNNER_CLIENTS_DELETER_HPP
#include <mutex>
#include <vector>
#include <memory>
#include "client.hpp"
namespace trustwave {
    struct clients_deleter {
        void push(std::shared_ptr<cdcm_client> c) { vec_.push_back(c); }
        void delete_clients() { vec_.clear(); }

    protected:
        std::vector<std::shared_ptr<cdcm_client>> vec_;
    };
}//namespace trustwave
#endif // SRC_COMMON_SINGLETON_RUNNER_CLIENTS_DELETER_HPP