//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														scoped_timer.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/16/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_UTILS_SCOPED_TIMER_HPP
#define SRC_COMMON_UTILS_SCOPED_TIMER_HPP
#include <string>
#include <chrono>
#include <iostream>
namespace trustwave{
    struct scoped_timer {
        explicit scoped_timer(const std::string_view name):
            start_(std::chrono::high_resolution_clock::now()), name_(name)
        {
        }
        ~scoped_timer()
        {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_);

            std::cerr << "Time taken by " << name_ << ": " << duration.count() << " microseconds" << std::endl;
        }
        decltype(std::chrono::high_resolution_clock::now()) start_;
        const std::string_view name_;
    };
}
#endif // SRC_COMMON_UTILS_SCOPED_TIMER_HPP