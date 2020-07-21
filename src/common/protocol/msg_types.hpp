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
    enum class RESP_GROUP_CODE  {OK = 1, ERROR_IN_REQUEST, CDCM_INTERNAL_ERROR, ERROR_WITH_ASSET, NON_EXIST};

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
        REMOTE_AND_USERNAME_ARE_MANDATORY,
        SHARE_IS_MISSING_IN_KEY,

        SESSION_NOT_FOUND = 301, //cdcm internal error
        INTERNAL_ERROR,
        MEMORY_ALLOCATION_FAILED,
        PARSE_FILE_FAILED,
        READ_FILE_FAILED,
        OPEN_KEY_FAILED,
        FAILED_ADDING_NEW_SESSION,

        LIST_FAILED = 401,        // error with asset
        GENERAL_ERROR_WITH_ASSET
    };


    static std::map<CDCM_ERROR, std::tuple<RESP_GROUP_CODE, uint32_t, std::string>> cdcm_error_mapping = {
            {CDCM_ERROR::OK , {RESP_GROUP_CODE::OK, 101, "OK"} },

            {CDCM_ERROR::MALFORMED_MESSAGE , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 201, "Malformed message"} },
            {CDCM_ERROR::KEY_AND_VALUE_ARE_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 202, "Key and value are mandatory"} },
            {CDCM_ERROR::KEY_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 203, "Key is mandatory"} },
            {CDCM_ERROR::BAD_PARAMETER , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 204, "Bad parameter"} },
            {CDCM_ERROR::PARAM_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 205, "Param is mandatory"} },
            {CDCM_ERROR::PATH_IS_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 206, "Path is mandatory"} },
            {CDCM_ERROR::REMOTE_AND_USERNAME_ARE_MANDATORY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 207, "Remote and username are mandatory"} },
            {CDCM_ERROR::SHARE_IS_MISSING_IN_KEY , {RESP_GROUP_CODE::ERROR_IN_REQUEST, 208, "Share is missing in the provided key"} },

            {CDCM_ERROR::SESSION_NOT_FOUND , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 301, "Session not found"} },
            {CDCM_ERROR::INTERNAL_ERROR , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 302, "Internal error"} },
            {CDCM_ERROR::MEMORY_ALLOCATION_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 303, "Memory allocation failed"} },
            {CDCM_ERROR::PARSE_FILE_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 304, "Parse file failed"}},
            {CDCM_ERROR::READ_FILE_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 305, "Read file failed"}},
            {CDCM_ERROR::OPEN_KEY_FAILED , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 306, "Open key failed"}},
            {CDCM_ERROR::FAILED_ADDING_NEW_SESSION , {RESP_GROUP_CODE::CDCM_INTERNAL_ERROR, 307, "Failed adding new session"}},

            {CDCM_ERROR::LIST_FAILED , {RESP_GROUP_CODE::ERROR_WITH_ASSET, 401, "List failed"}},
            {CDCM_ERROR::GENERAL_ERROR_WITH_ASSET , {RESP_GROUP_CODE::ERROR_WITH_ASSET, 402, "Error with asset"}}

    };
    //rotem TODO: change struct name
    struct resp_status {
    public:
        resp_status(RESP_GROUP_CODE grp_code, uint32_t code) {
            code_group(grp_code);
            code_ = code;
        }
        resp_status(const resp_status&) = default;
        resp_status(resp_status&&) = default;
        resp_status& operator=(const resp_status&) = default;
        resp_status& operator=(resp_status&&) = default;
        resp_status() = default;

        /************************************************************************************
         * set both the code_group and the code_group_description
         ************************************************************************************/
        void code_group(RESP_GROUP_CODE resp_group_code) {
            code_group_ = (uint32_t)resp_group_code;
            code_group_description_ = group_code_description_mapping[resp_group_code];
        }

        void code(uint32_t code) { code_ = code;}

        /************************************************************************************
         * sets 'code_group', 'code_group_description' and 'code'
         ************************************************************************************/
        void set_resp_status_for_cdcm_error(CDCM_ERROR cdcm_error)
        {
            code_group(std::get<0>(cdcm_error_mapping[cdcm_error]));
            code(std::get<1>(cdcm_error_mapping[cdcm_error]));
        };

        std::string get_error_message(CDCM_ERROR& cdcm_error)
        {
            return std::get<2>(cdcm_error_mapping[cdcm_error]);
        }

        uint32_t code_group_ ;
        std::string code_group_description_ ;
        uint32_t code_ ;
    };

    // todo assaf fix encapsulation
    struct result_msg {
    public:
        std::string id_;
        resp_status resp_status_;
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

        /************************************************************************************
         * sets both the status (for success) and res
         ************************************************************************************/
        void set_response_for_success(const tao::json::value& result) {
            resp_status_.set_resp_status_for_cdcm_error(CDCM_ERROR::OK);
            res(result);
        }

        /***********************************************************************************
        *  sets both the status (for error) and res (with the appropriate error message)
        ************************************************************************************/
        void set_response_for_error(CDCM_ERROR cdcm_error) {
            resp_status_.set_resp_status_for_cdcm_error(cdcm_error);
            res("Error: " + resp_status_.get_error_message(cdcm_error));
        }



        /************************************************************************************
        * sets 'code_group', 'code_group_description' related to the cdcm_error enum.
        * 'code' and 'res' will be taken from input parameters. if not supplied, 'code' and 'res' will
        * be taken from the code and string related to the cdcm_error enum
        ************************************************************************************/
        void set_response_for_error_with_unique_code_or_msg(CDCM_ERROR cdcm_error, uint32_t error_code =0, std::string err_msg= "") {
            resp_status_.code_group(std::get<0>(cdcm_error_mapping[cdcm_error]));
            if (0 != error_code)
                resp_status_.code(error_code);
            else
                resp_status_.code(std::get<1>(cdcm_error_mapping[cdcm_error]));

            if (!err_msg.empty())
                res("Error: " + err_msg);
            else
                res("Error: " + resp_status_.get_error_message(cdcm_error));
        }

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
