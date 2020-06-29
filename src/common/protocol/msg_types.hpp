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
#include <utility>
#include <vector>
#include <map>

#include <taocpp-json/include/tao/json/value.hpp>

namespace trustwave {
    enum class RESP_GROUP_CODE  {OK = 1, ERROR_IN_REQUEST, CDCM_INTERNAL_ERROR, ERROR_WITH_ASSET};
    static  std::map<RESP_GROUP_CODE,std::string> group_code_description_mapping = {{RESP_GROUP_CODE::OK ,"OK"},
                                                                                    {RESP_GROUP_CODE::ERROR_IN_REQUEST, "Error in request"},
                                                                                    {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, "CDCM internal error"},
                                                                                    {RESP_GROUP_CODE::ERROR_WITH_ASSET, "Error with asset"}};


    enum class  CDCM_ERROR : uint32_t
    {
        OK = 101,

        MALFORMED_MESSAGE = 201, //error in request
        KEY_AND_VALUE_ARE_MANDATORY,
        KEY_IS_MANDATORY,
        BAD_PARAMETER,
        PARAM_IS_MANDATORY,
        PATH_IS_MANDATORY,

        SESSION_NOT_FOUND = 301, //cdcm internal error
        INTERNAL_ERROR,
        MEMORY_ALLOCATION_FAILED,
        PARSE_FILE_FAILED,

        ERROR_WITH_ASSET = 401
    };


    std::map<CDCM_ERROR, std::tuple<RESP_GROUP_CODE, uint32_t, std::string>> cdcm_error_mapping = {
            {CDCM_ERROR::OK , {RESP_GROUP_CODE::OK, 101, "OK"} },

            {CDCM_ERROR::MALFORMED_MESSAGE , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 201, "Malformed message"} },
            {CDCM_ERROR::KEY_AND_VALUE_ARE_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 202, "Key and value are mandatory"} },
            {CDCM_ERROR::KEY_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 203, "Key is mandatory"} },
            {CDCM_ERROR::BAD_PARAMETER , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 204, "Bad parameter"} },
            {CDCM_ERROR::PARAM_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 205, "Param is mandatory"} },
            {CDCM_ERROR::PATH_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 206, "Path is mandatory"} },

            {CDCM_ERROR::SESSION_NOT_FOUND , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 301, "Session not found"} },
            {CDCM_ERROR::INTERNAL_ERROR , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 302, "Internal error"} },
            {CDCM_ERROR::MEMORY_ALLOCATION_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 303, "Memory allocation failed"} },
            {CDCM_ERROR::PARSE_FILE_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 304, "Parse file failed"} }

    };
    struct resp_code {
    public:
        resp_code(RESP_GROUP_CODE grp_code, uint32_t err_code) {
            code_group(grp_code);
            error_code_ = err_code;
        }
        resp_code(const resp_code&) = default;
        resp_code(resp_code&&) = default;
        resp_code& operator=(const resp_code&) = default;
        resp_code& operator=(resp_code&&) = default;
        resp_code() = default;
        void code_group(RESP_GROUP_CODE resp_group_code) {
            code_group_ = (uint32_t)resp_group_code;
            code_group_description_ = group_code_description_mapping[resp_group_code];
        }
        void error_code(uint32_t error_code) {error_code_ = error_code;}

        void set_resp_status_for_cdcm_error(CDCM_ERROR cdcm_error)
        {
            code_group(std::get<0>(cdcm_error_mapping[cdcm_error]));
            error_code(std::get<1>(cdcm_error_mapping[cdcm_error]));
        };

        std::string get_error_message

        uint32_t code_group_;
        std::string code_group_description_;
        uint32_t error_code_;
    };

    // todo assaf fix encapsulation
    struct result_msg {
    public:
        std::string id_;
        resp_code resp_code_;
        tao::json::value res_;

    public:
        virtual ~result_msg() = default;
        result_msg(const result_msg&) = default;
        result_msg(result_msg&&) = default;
        result_msg& operator=(const result_msg&) = default;
        result_msg& operator=(result_msg&&) = default;
        result_msg() = default;
        std::string id() const { return id_; }
        void id(const std::string& ids) { id_ = ids; }
        tao::json::value res() const { return res_; }
        void res(const tao::json::value& ress) { res_ = ress; }
        resp_code get_resp_code() const { return resp_code_; }
        void set_resp_code(const resp_code& resp_code) { resp_code_ = resp_code; }
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
        header(const header&) = default;
        header(header&&) = default;
        header& operator=(const header&) = default;
        header& operator=(header&&) = default;
        header() = default;
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
