//===========================================================================
// Trustwave ltd. @{SRCH}
//								close_session.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 4 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_
#define TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_
//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "action.hpp"
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave{
    struct local_close_session_msg: public action_msg
    {
        static constexpr std::string_view act_name{"close_session"};
        local_close_session_msg() :
                action_msg(act_name)
        {
        }

    };
}
namespace tao::json
{
    template<>
    struct traits<trustwave::local_close_session_msg>
            : binding::object<binding::inherit<traits<trustwave::action_msg> >> {
        TAO_JSON_DEFAULT_KEY(trustwave::local_close_session_msg::act_name.data());
        template< template< typename... > class Traits >
        static trustwave::local_close_session_msg as( const tao::json::basic_value< Traits >&  )
        {
            trustwave::local_close_session_msg result;
            return result;
        }
    };
}
namespace trustwave {

class Close_Session: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Close_Session() :
                    Action_Base(trustwave::local_close_session_msg::act_name)
    {
    }
    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
    [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
    {
        return v.as<std::shared_ptr<local_close_session_msg>>();
    }

};

}

#endif /* TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_ */
