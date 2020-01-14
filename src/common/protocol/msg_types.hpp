//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														msg_types.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 May 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_PROTOCOL_MSG_TYPES_HPP_
#define TRUSTWAVE_COMMON_PROTOCOL_MSG_TYPES_HPP_
#include <memory>
#include <string>
#include <taocpp-json/include/tao/json/value.hpp>
#include <utility>
#include <vector>
namespace trustwave {
    // todo assaf fix encapsulation
    struct result_msg {
    public:
        std::string id_;
        std::string res_;

    public:
        virtual ~result_msg() = default;
        result_msg(const result_msg&) = default;
        result_msg(result_msg&&) = default;
        result_msg& operator=(const result_msg&) = default;
        result_msg& operator=(result_msg&&) = default;
        result_msg() = default;
        std::string id() const { return id_; }
        void id(const std::string& ids) { id_ = ids; }
        std::string res() const { return res_; }
        void res(const std::string& ress) { res_.assign(ress); }
    };

    struct action_msg {
        virtual ~action_msg() = default;
        action_msg(const action_msg&) = default;
        action_msg(action_msg&&) = default;
        action_msg& operator=(const action_msg&) = delete;
        action_msg& operator=(action_msg&&) = delete;
        action_msg() = delete;
        std::string name() const { return std::move(std::string(name_)); }
        std::string id() const { return id_; }
        void id(const std::string& ids) { id_ = ids; }

        std::string id_;
        const std::string_view name_;

    protected:
        explicit action_msg(const std::string_view name): name_(name) {}
    };
    struct single_param_action_msg: public action_msg {
        single_param_action_msg() = delete;

    protected:
        explicit single_param_action_msg(const std::string_view name): action_msg(name) {}

        single_param_action_msg(const single_param_action_msg& o, const std::string_view& name): action_msg(name)
        {
            id_ = o.id_;
            param = o.param;
        }

    public:
        std::string param;
    };

    struct header {
        std::string session_id;
    };
    struct msg {
        header hdr;
        std::vector<std::shared_ptr<action_msg>> msgs;
        virtual ~msg() = default;
        msg(const msg&) = default;
        msg(msg&&) = default;
        msg& operator=(const msg&) = default;
        msg& operator=(msg&&) = default;
        msg() = default;
    };
    struct raw_msg {
        header hdr;
        std::vector<std::map<std::string, tao::json::value, std::less<>>> msgs;

        virtual ~raw_msg() = default;

        raw_msg(const raw_msg&) = default;

        raw_msg(raw_msg&&) = default;

        raw_msg& operator=(const raw_msg&) = default;

        raw_msg& operator=(raw_msg&&) = default;

        raw_msg() = default;
    };
    struct res_msg {
        header hdr;
        std::vector<std::shared_ptr<result_msg>> msgs;
        virtual ~res_msg() = default;
        res_msg(const res_msg&) = default;
        res_msg(res_msg&&) = default;
        res_msg& operator=(const res_msg&) = default;
        res_msg& operator=(res_msg&&) = default;
        res_msg() = default;
    };

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_PROTOCOL_MSG_TYPES_HPP_ */
