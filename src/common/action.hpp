//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														action.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 14 May 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_ACTION_HPP_
#define TRUSTWAVE_COMMON_ACTION_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "dispatcher.hpp"
#include <boost/core/noncopyable.hpp> // for noncopyable
#include <boost/shared_ptr.hpp>
#include <memory>
#include <string> // for string
#include <taocpp-json/include/tao/json/forward.hpp>
#include <utility>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================

namespace trustwave {

    class session;
    struct action_msg;
    struct result_msg;
    class Action_Base {
    public:
        enum class action_status : int8_t {
            FAILED = -1,
            SUCCEEDED = 0,
            POSTPONED = 1,
        };
        explicit Action_Base(const std::string_view name): name_(name) {}

        virtual ~Action_Base() = default;
        Action_Base(const Action_Base&) = delete;
        Action_Base& operator=(const Action_Base&) = delete;
        Action_Base(const Action_Base&&) = delete;
        Action_Base& operator=(const Action_Base&&) = delete;
        [[nodiscard]] std::string_view name() const { return name_; }

        virtual action_status
        act(boost::shared_ptr<session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>)
            = 0;
        [[nodiscard]] virtual std::shared_ptr<action_msg> get_message(const tao::json::value& v) const = 0;

    private:
        const std::string_view name_;
    };
} // namespace trustwave
using import_action_cb_t = std::shared_ptr<trustwave::Action_Base>();

extern "C" {
extern std::shared_ptr<trustwave::Action_Base> import_action();
}

#endif /* TRUSTWAVE_COMMON_ACTION_HPP_ */
