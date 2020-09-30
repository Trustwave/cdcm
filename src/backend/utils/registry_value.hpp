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
#include "taocpp-json/include/tao/json.hpp"
#include "taocpp-json/include/tao/json/contrib/traits.hpp"
namespace trustwave {
    enum reg_type:uint32_t
    {
         REG_NONE                         =0 ,
         REG_SZ                           =1 ,
         REG_EXPAND_SZ                    =2 ,
         REG_BINARY                       =3 ,
         REG_DWORD                        =4 ,
         REG_DWORD_BIG_ENDIAN             =5 ,
         REG_LINK                         =6 ,
         REG_MULTI_SZ                     =7 ,
         REG_RESOURCE_LIST                =8 ,
         REG_FULL_RESOURCE_DESCRIPTOR     =9 ,
         REG_RESOURCE_REQUIREMENTS_LIST   =10,
         REG_QWORD                        =11
    };
    static std::map<uint32_t, std::string> reg_type_string = {
        {REG_NONE                      ,"REG_NONE"                        },
        { REG_SZ                        ,"REG_SZ"                         },
        { REG_EXPAND_SZ                 ,"REG_EXPAND_SZ"                  },
        { REG_BINARY                    ,"REG_BINARY"                     },
        { REG_DWORD                     ,"REG_DWORD"                      },
        { REG_DWORD_BIG_ENDIAN          ,"REG_DWORD_BIG_ENDIAN"           },
        { REG_LINK                      ,"REG_LINK"                       },
        { REG_MULTI_SZ                  ,"REG_MULTI_SZ"                   },
        { REG_RESOURCE_LIST             ,"REG_RESOURCE_LIST"              },
        { REG_FULL_RESOURCE_DESCRIPTOR  ,"REG_FULL_RESOURCE_DESCRIPTOR"   },
        { REG_RESOURCE_REQUIREMENTS_LIST,"REG_RESOURCE_REQUIREMENTS_LIST" },
        { REG_QWORD                     ,"REG_QWORD"                      }
    };

    class registry_value final {
    public:
        registry_value(): value_() { }
        registry_value(uint32_t type, const std::string& value,const std::string& name): type_(type),type_as_string_(reg_type_string[type]), value_(value), name_(name) { }
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
        explicit sub_key(const std::string& name):name_(name){}
        sub_key(const std::string& name,
                         const std::string& class_name,
                         const std::string& last_modified):name_(name),class_name_(class_name),last_modified_(last_modified)
        {}
        std::string name_;
        std::string class_name_;
        std::string last_modified_;
    };
    using enum_key_values=std::vector<registry_value> ;//fixme assaf remove duplicate


    struct enum_key {
        std::vector<sub_key> sub_keys_;
        enum_key_values registry_values_;
    };

} // namespace trustwave
namespace tao ::json {

    template<>
    struct traits<trustwave::registry_value>:
        binding::object<TAO_JSON_BIND_REQUIRED("name", &trustwave::registry_value::name_),
            TAO_JSON_BIND_REQUIRED("type", &trustwave::registry_value::type_as_string_),
            TAO_JSON_BIND_REQUIRED("value", &trustwave::registry_value::value_)> {
    };
    template<>
    struct traits<trustwave::sub_key>:
        binding::object<TAO_JSON_BIND_REQUIRED("name", &trustwave::sub_key::name_),
            TAO_JSON_BIND_REQUIRED("class_name", &trustwave::sub_key::class_name_),
            TAO_JSON_BIND_REQUIRED("last_modified", &trustwave::sub_key::last_modified_)> {
    };

    template<>
    struct traits<trustwave::enum_key>:
        binding::object<TAO_JSON_BIND_REQUIRED("sub_keys", &trustwave::enum_key::sub_keys_),
            TAO_JSON_BIND_REQUIRED("registry_values", &trustwave::enum_key::registry_values_)> {
    };
} // namespace tao::json
#endif /* TRUSTWAVE_MISC_REGISTRY_VALUE_HPP_ */
