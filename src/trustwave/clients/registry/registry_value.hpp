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
namespace trustwave {
class registry_value
{
public:
    registry_value() :
                    type_(0), value_()
    {
    }
    registry_value(uint32_t type, const char* value) :
                    type_(type), value_(value)
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
    void type(uint32_t type)
    {
        type_ = type;
    }
    void value(std::string value)
    {
        value_ = value;
    }
private:
    uint32_t type_;
    std::string value_;
};

}
#endif /* TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_ */
