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
namespace trustwave {
//todo assaf fix encapsulation
struct result_msg
{
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
    std::string id() const
    {
        return id_;
    }
    void id(const std::string& ids)
    {
        id_ = ids;
    }
    std::string res() const
    {
        return res_;
    }
    void res(const std::string& ress)
    {
        res_ = ress;
    }
};

struct action_msg
{
    virtual ~action_msg() = default;
    action_msg(const action_msg&) = default;
    action_msg(action_msg&&) = default;
    action_msg& operator=(const action_msg&) = delete;
    action_msg& operator=(action_msg&&) = delete;
    action_msg() = delete;
    const std::string& name() const
    {
        return name_;
    }
    std::string id() const
    {
        return id_;
    }
    void id(const std::string& ids)
    {
        id_ = ids;
    }

    std::string id_;
    const std::string name_;

protected:
    explicit action_msg(std::string  name) :
                    name_(std::move(name))
    {
    }

};

struct reg_action_query_value_msg: public action_msg
{
    reg_action_query_value_msg() :
                    action_msg("query_value")
    {
    }
    std::string key_;
    std::string value_;

};
struct reg_action_get_os_msg: public action_msg
{
    reg_action_get_os_msg() :
            action_msg("get_os")
    {
    }
};
struct reg_action_enum_key_msg: public action_msg
{
    reg_action_enum_key_msg() :
                    action_msg("enumerate")
    {
    }
    std::string key_;

};
struct reg_action_value_exists_msg: public action_msg
    {
        reg_action_value_exists_msg() :
                action_msg("value_exists")
        {
        }
        std::string key_;
        std::string value_;
    };
struct reg_action_key_exists_msg: public action_msg
{
    reg_action_key_exists_msg() :
                    action_msg("key_exists")
    {
    }
    std::string key_;

};
struct local_start_session_msg: public action_msg
{

    local_start_session_msg() :
                    action_msg("start_session")
    {
    }
    std::string remote;
    std::string domain;
    std::string username;
    std::string password;
    std::string workstation;
};

struct local_close_session_msg: public action_msg
{

    local_close_session_msg() :
                    action_msg("close_session")
    {
    }

};
struct single_param_action_msg: public action_msg
{
protected:
    single_param_action_msg() = delete;
    explicit single_param_action_msg(const std::string& name) :
                    action_msg(name)
    {
    }
    single_param_action_msg(const single_param_action_msg& o, const std::string& name) :
                    action_msg(name)
    {
        id_ = o.id_;
        param = o.param;
    }
public:
    std::string param;
};
struct smb_get_file_info_msg: public single_param_action_msg
    {

        smb_get_file_info_msg() :
                single_param_action_msg("get_file_info")
        {
        }
        explicit smb_get_file_info_msg(const single_param_action_msg& o) :
                single_param_action_msg(o, "get_file_info")
        {
        }
    };
/*
struct smb_get_file_msg: public single_param_action_msg
{

    smb_get_file_msg() :
                    single_param_action_msg("get_file")
    {
    }
    smb_get_file_msg(const single_param_action_msg& o) :
                    single_param_action_msg(o, "get_file")
    {
    }
};
*/
struct smb_list_dir_msg: public single_param_action_msg
{

    smb_list_dir_msg() :
            single_param_action_msg("list_dir")
    {
    }
    explicit smb_list_dir_msg(const single_param_action_msg& o) :
            single_param_action_msg(o, "list_dir")
    {
    }
    std::string pattern;
};
/*
struct get_remote_file_version_msg: public single_param_action_msg
{

    get_remote_file_version_msg() :
                    single_param_action_msg("get_remote_file_version")
    {
    }
    get_remote_file_version_msg(const single_param_action_msg& o) :
                    single_param_action_msg(o, "get_remote_file_version")
    {
    }
};
struct local_get_file_version_msg: public single_param_action_msg
{

    local_get_file_version_msg() :
                    single_param_action_msg("local_get_file_version_msg")
    {
    }
    local_get_file_version_msg(const single_param_action_msg& o) :
                    single_param_action_msg(o, "local_get_file_version_msg")
    {
    }
};
*/
struct smb_read_file_msg: public action_msg
    {
        smb_read_file_msg() :
                action_msg("read_file")
        {
        }
        std::string path_;
        std::string size_;
        std::string offset_;
    };

struct header
{
    std::string session_id;
};
struct msg
{
    header hdr;
    std::vector<std::shared_ptr<action_msg>> msgs;
    virtual ~msg() = default;
    msg(const msg&) = default;
    msg(msg&&) = default;
    msg& operator=(const msg&) = default;
    msg& operator=(msg&&) = default;
    msg() = default;
};

struct res_msg
{
    header hdr;
    std::vector<std::shared_ptr<result_msg>> msgs;
    virtual ~res_msg() = default;
    res_msg(const res_msg&) = default;
    res_msg(res_msg&&) = default;
    res_msg& operator=(const res_msg&) = default;
    res_msg& operator=(res_msg&&) = default;
    res_msg() = default;
};
}

#endif /* TRUSTWAVE_COMMON_PROTOCOL_MSG_TYPES_HPP_ */
