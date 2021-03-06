//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														registry_value.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 29 Apr 2019
// Comments:

#ifndef TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_
#define TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_
#include <string>
#include <vector>
#include <map>

namespace trustwave {

    static std::map<uint32_t, std::string> reg_type_string = {
            {0,"REG_NONE"},
            {1,"REG_SZ"},
            {2,"REG_EXPAND_SZ"},
            {3,"REG_BINARY"},
            {4,"REG_DWORD"},
            {5,"REG_DWORD_BIG_ENDIAN"},
            {6,"REG_LINK"},
            {7,"REG_MULTI_SZ"},
            {8,"REG_RESOURCE_LIST"},
            {9,"REG_FULL_RESOURCE_DESCRIPTOR"},
            {10,"REG_RESOURCE_REQUIREMENTS_LIST"},
            {11,"REG_QWORD"}
    };

    class registry_value final {
    public:
        registry_value(): value_() { }
        registry_value(uint32_t type, const char* value, const char* name): type_(type), value_(value), name_(name) { }
        uint32_t type() const { return type_; }
        std::string value() const { return value_; }
        std::string name() const { return name_; }
        void type(uint32_t type) {
            type_ = type;
            type_as_string_ = reg_type_string[type];
        }
        void value(const std::string& value) { value_ = value; }
        void name(const std::string& name) { name_ = name; }
        // private:
        uint32_t type_{0};
        std::string type_as_string_;
        std::string value_;
        std::string name_;
    };
    struct sub_key {
        std::string name_;
        std::string class_name_;
        std::string last_modified_;
    };
    struct enum_key {
        std::vector<sub_key> sub_keys_;
        std::vector<registry_value> registry_values_;
    };

    struct enum_key_values {
        std::vector<registry_value> registry_values_;
    };

} // namespace trustwave
#endif /* TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_ */
