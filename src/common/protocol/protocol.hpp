//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														protocol.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 May 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_PROTOCOL_PROTOCOL_HPP_
#define TRUSTWAVE_COMMON_PROTOCOL_PROTOCOL_HPP_
#include "msg_types.hpp"
#include <taocpp-json/include/tao/json.hpp>
#include <taocpp-json/include/tao/json/contrib/traits.hpp>

namespace tao::json {

    template<>
    struct traits<trustwave::header>:
        binding::object<TAO_JSON_BIND_REQUIRED("session_id", &trustwave::header::session_id)> {
    };

    template<>
    struct traits<trustwave::msg>:
        binding::object<TAO_JSON_BIND_REQUIRED("H", &trustwave::msg::hdr),
                        TAO_JSON_BIND_REQUIRED("msgs", &trustwave::msg::msgs)> {
    };
    template<>
    struct traits<trustwave::raw_msg>:
        binding::object<TAO_JSON_BIND_REQUIRED("H", &trustwave::raw_msg::hdr),
                        TAO_JSON_BIND_REQUIRED("msgs", &trustwave::raw_msg::msgs)> {
    };
    template<>
    struct traits<trustwave::res_msg>:
        binding::object<TAO_JSON_BIND_REQUIRED("H", &trustwave::res_msg::hdr),
                        TAO_JSON_BIND_REQUIRED("msgs", &trustwave::res_msg::msgs)> {
    };

    template<>
    struct traits<trustwave::result_msg>:
        binding::object<TAO_JSON_BIND_REQUIRED("res", &trustwave::result_msg::res_),
                        TAO_JSON_BIND_REQUIRED("id", &trustwave::result_msg::id_)> {
    };

    template<>
    struct traits<trustwave::action_msg>:
        binding::object<

            TAO_JSON_BIND_REQUIRED("id", &trustwave::action_msg::id_)> {
    };
    template<>
    struct traits<trustwave::single_param_action_msg>:
        binding::object<binding::inherit<traits<trustwave::action_msg>>,
                        TAO_JSON_BIND_REQUIRED("param", &trustwave::single_param_action_msg::param)> {
    };
} // namespace tao::json
#endif /* TRUSTWAVE_COMMON_PROTOCOL_PROTOCOL_HPP_ */
