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
namespace trustwave {
class registry_value
{
public:
    registry_value() :
                    type_(0), value_()
    {
    }
    registry_value(uint32_t type, const char* value,const char* name) :
                    type_(type), value_(value),name_(name)
    {
    }
    uint32_t type() const
    {
        return type_;
    }
    std::string value() const
    {
        return value_;
    }
    std::string name() const
    {
        return name_;
    }
    void type(uint32_t type)
    {
        type_ = type;
    }
    void value(std::string value)
    {
        value_ = value;
    }
    void name(std::string name)
    {
        name_ = name;
    }
//private:
    uint32_t type_;
    std::string value_;
    std::string name_;
};
struct sub_key
{
    std::string name_;
    std::string class_name_;
    std::string last_modified_;
};
struct enum_key
{
    std::vector<sub_key> sub_keyes_;
    std::vector<registry_value> registry_values_;
};

}
#endif /* TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_ */
