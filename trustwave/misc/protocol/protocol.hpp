/*
 * protocol.hpp
 *
 *  Created on: May 15, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_PROTOCOL_PROTOCOL_HPP_
#define TRUSTWAVE_MISC_PROTOCOL_PROTOCOL_HPP_
#include "json/include/tao/json/contrib/traits.hpp"
#include "json/include/tao/json.hpp"
#include "msg_types.hpp"

namespace tao {
namespace json {

template<>
struct traits<trustwave::header> : binding::object< TAO_JSON_BIND_REQUIRED ("session_id", &trustwave::header::session_id)>
{
};

template<>
struct traits<trustwave::msg> : binding::object< TAO_JSON_BIND_REQUIRED ("H",&trustwave::msg::hdr),
                                                 TAO_JSON_BIND_REQUIRED("msgs", &trustwave::msg::msgs)>
{
};

template<>
struct traits<trustwave::res_msg> : binding::object< TAO_JSON_BIND_REQUIRED ("H",&trustwave::res_msg::hdr),
                                                 TAO_JSON_BIND_REQUIRED("msgs", &trustwave::res_msg::msgs)>
{
};

template<>
struct traits<trustwave::result_msg> : binding::object<
TAO_JSON_BIND_REQUIRED("res", &trustwave::result_msg::res_),
TAO_JSON_BIND_REQUIRED("id", &trustwave::result_msg::id_)>
{
};

template<>
struct traits<trustwave::action_msg> : binding::object<

TAO_JSON_BIND_REQUIRED("id", &trustwave::action_msg::id_)>
{
};

template<>
struct traits<std::shared_ptr<trustwave::action_msg> > : binding::factory< TAO_JSON_FACTORY_BIND1( trustwave::smb_get_file_msg ),
TAO_JSON_FACTORY_BIND1(trustwave::reg_action_query_value_msg ),TAO_JSON_FACTORY_BIND1(trustwave::local_start_session_msg ) ,
TAO_JSON_FACTORY_BIND1(trustwave::get_remote_file_version_msg )>
{
};

template<>
struct traits<trustwave::single_param_action_msg> : binding::object<binding::inherit<traits<trustwave::action_msg> >,
                                        TAO_JSON_BIND_REQUIRED( "param", &trustwave::single_param_action_msg::param ) >
{

};


template<>
struct traits<trustwave::smb_get_file_msg> : binding::object<binding::inherit<traits<trustwave::single_param_action_msg> > >
{
    TAO_JSON_DEFAULT_KEY( "get_file" );
};
template<>
struct traits<trustwave::get_remote_file_version_msg> : binding::object<binding::inherit<traits<trustwave::single_param_action_msg> >>
{
    TAO_JSON_DEFAULT_KEY( "get_remote_file_version" );
};
template<>
struct traits<trustwave::local_start_session_msg> : binding::object<binding::inherit<traits<trustwave::action_msg> >,
                                        TAO_JSON_BIND_REQUIRED( "remote", &trustwave::local_start_session_msg::remote ),
                                        TAO_JSON_BIND_REQUIRED( "domain", &trustwave::local_start_session_msg::domain ),
                                        TAO_JSON_BIND_REQUIRED( "username", &trustwave::local_start_session_msg::username ),
                                        TAO_JSON_BIND_REQUIRED( "password", &trustwave::local_start_session_msg::password ),
                                        TAO_JSON_BIND_REQUIRED( "workstation", &trustwave::local_start_session_msg::workstation )>
{
    TAO_JSON_DEFAULT_KEY( "start_session" );
};
template<>
struct traits<trustwave::reg_action_query_value_msg> : binding::object<binding::inherit<traits<trustwave::action_msg> >,
TAO_JSON_BIND_REQUIRED( "key", &trustwave::reg_action_query_value_msg::key_ ),
TAO_JSON_BIND_REQUIRED( "value", &trustwave::reg_action_query_value_msg::value_ ) >
{
    TAO_JSON_DEFAULT_KEY( "query_value" );
};
}
}

#endif /* TRUSTWAVE_MISC_PROTOCOL_PROTOCOL_HPP_ */
