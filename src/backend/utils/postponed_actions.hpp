//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														postponed_actions.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 3/1/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_UTILS_POSTPONED_ACTIONS_HPP
#define SRC_BACKEND_UTILS_POSTPONED_ACTIONS_HPP
#include <string>
#include <memory>
#include <boost/multi_index_container.hpp> // for interpro...
#include <boost/multi_index/ordered_index.hpp> //
#include <boost/multi_index/mem_fun.hpp> // for multi_index
#include <boost/multi_index/member.hpp> // for multi_index
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp> // for interpro...
#include <boost/multi_index/ordered_index.hpp> //
#include <boost/multi_index/mem_fun.hpp> // for multi_index
#include <boost/multi_index/member.hpp> // for multi_index
#include "protocol/msg_types.hpp"

namespace trustwave {
    class postponed_action;
    std::ostream& operator<<(std::ostream&, const postponed_action&);
    namespace bmi = boost::multi_index;
    namespace chr = std::chrono;
    class postponed_action final {
        friend std::ostream& ::trustwave::operator<<(std::ostream&, const postponed_action&);

    public:
        postponed_action(tao::json::value action_message, std::string reply_to, std::string id, u_int8_t remaining_runs,
                         time_t expiration_time, header hdr):
            action_message_(action_message),
            reply_to_(reply_to), id_(id), remaining_runs_(remaining_runs), expiration_time_(expiration_time),
            header_(hdr)
        {
        }
        postponed_action() = default;
        postponed_action(const postponed_action&) = default;
        postponed_action(postponed_action&&) = default;
        postponed_action& operator=(const postponed_action&) = default;
        postponed_action& operator=(postponed_action&&) = default;
        ~postponed_action() = default;
        time_t expiration_time() const { return expiration_time_; }
        void expiration_time(time_t nt) { expiration_time_ = nt; }
        tao::json::value msg() const { return action_message_; }
        uint8_t remaining_runs() const { return remaining_runs_; }
        void decrement_runs() { remaining_runs_--; }
        std::string reply_to() const { return reply_to_; }
        std::string id() const { return id_; }
        header get_hdr() const { return header_; }

    private:
        tao::json::value action_message_;
        std::string reply_to_;
        std::string id_;
        u_int8_t remaining_runs_;
        time_t expiration_time_;
        header header_;
    };
    class postponed_actions_queue;
    std::ostream& operator<<(std::ostream&, const postponed_actions_queue&);
    class postponed_actions_queue final {
        friend std::ostream& operator<<(std::ostream&, const postponed_actions_queue&);

    public:
        [[nodiscard]] bool decrement_runs_and_update_expiration(const std::string& ids)
        {
            auto pa = paq_.get<id>().find(ids);
            if(pa == paq_.get<id>().end()) {
                return false;
            }
            auto it2 = paq_.project<expiration>(pa);
            paq_.get<expiration>().modify(it2, [this](postponed_action& x) {
                x.expiration_time(chr::system_clock::to_time_t(chr::system_clock::now()) + 1);
                x.decrement_runs();
            });
            return true;
        }
        [[nodiscard]] bool remove_by_id(const std::string& ids)
        {
            if(paq_.get<id>().find(ids) == paq_.get<id>().end()) {
                return false;
            }
            paq_.get<id>().erase(ids);
            return true;
        }
        [[nodiscard]] bool add(const postponed_action& pa) { return paq_.insert(pa).second; }
        [[nodiscard]] const postponed_action& top()
        {
            return (*(paq_.get<expiration>().cbegin())); // beware of empty
        }
        [[nodiscard]] bool empty() const noexcept { return paq_.empty(); }

    private:
        // Tags
        struct id;
        struct expiration;

        typedef bmi::multi_index_container<
            postponed_action,
            bmi::indexed_by<
                bmi::ordered_unique<boost::multi_index::tag<id>,
                                    bmi::const_mem_fun<postponed_action, std::string, &postponed_action::id>>,
                bmi::ordered_non_unique<bmi::tag<expiration>, bmi::const_mem_fun<postponed_action, time_t,
                                                                                 &postponed_action::expiration_time>>>>
            p_actions;

        p_actions paq_;
    };

} // namespace trustwave

#endif // SRC_BACKEND_UTILS_POSTPONED_ACTIONS_HPP