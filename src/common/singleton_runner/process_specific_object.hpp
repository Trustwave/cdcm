//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														process_specific_object.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/27/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_SINGLETON_RUNNER_PROCESS_SPECIFIC_OBJECT_HPP
#define SRC_COMMON_SINGLETON_RUNNER_PROCESS_SPECIFIC_OBJECT_HPP
#include <string_view>
namespace trustwave {
    class process_specific_object {
    public:
        explicit process_specific_object(const std::string_view name): name_(name) { }
        virtual ~process_specific_object() = default;
        process_specific_object(const process_specific_object&) = delete;
        process_specific_object& operator=(const process_specific_object&) = delete;
        process_specific_object(const process_specific_object&&) = delete;
        process_specific_object& operator=(const process_specific_object&&) = delete;
        [[nodiscard]] std::string_view name() const { return name_; }

    private:
        const std::string_view name_;
    };
}// namespace trustwave
#endif // SRC_COMMON_SINGLETON_RUNNER_PROCESS_SPECIFIC_OBJECT_HPP