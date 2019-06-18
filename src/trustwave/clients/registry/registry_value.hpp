/*
 * registry_value.hpp
 *
 *  Created on: Apr 22, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_
#define TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_
#include <string>
namespace trustwave{
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
