//===========================================================================
// Trustwave ltd. @{SRCH}
//								key_exists.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 7 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_

//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "registry_action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave{
    struct reg_action_key_exists_msg : public action_msg {
        static constexpr std::string_view act_name{"key_exists"};
        reg_action_key_exists_msg() :
                action_msg(act_name) {
        }
        std::string key_;

    };
}
namespace tao::json {
    template<>
    struct traits<trustwave::reg_action_key_exists_msg>
            : binding::object<binding::inherit<traits<trustwave::action_msg> >,
                    TAO_JSON_BIND_REQUIRED("key", &trustwave::reg_action_key_exists_msg::key_) > {
        TAO_JSON_DEFAULT_KEY(trustwave::reg_action_key_exists_msg::act_name.data());


        template< template< typename... > class Traits >
        static trustwave::reg_action_key_exists_msg as( const tao::json::basic_value< Traits >& v )
        {
            trustwave::reg_action_key_exists_msg result;
            const auto o = v.at(trustwave::reg_action_key_exists_msg::act_name);
            result.key_ = o.at( "key" ).template as< std::string >();
            return result;
        }
    };
}
namespace trustwave {

class Key_Exists_Action: public Registry_Action
{

public:
    Key_Exists_Action() :
        Registry_Action( trustwave::reg_action_key_exists_msg::act_name)
    {
    }

    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
    [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
    {
        return v.as<std::shared_ptr<trustwave::reg_action_key_exists_msg>>();
    }
};

}

#endif /* TRUSTWAVE_SERVICES_REGISTRY_KEY_EXISTS_HPP_ */
