//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wql.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : rfrenkel
// Date    : 11/5/2019
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_ACTIONS_WQL_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_ACTIONS_WQL_HPP
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "action.hpp" //rotem TODO: check if ../../ can be removed
#include "protocol/msg_types.hpp"
#include "protocol/protocol.hpp"

//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

    struct wmi_action_wql_query_msg: public action_msg
    {
        static constexpr std::string_view act_name{"wmi_wql"};

        wmi_action_wql_query_msg() : action_msg(act_name) {

        }


        std::string wql;
        std::string wmi_namespace;

    };
} // end of namespace trustwave

namespace tao::json {

    template<>
    struct traits<trustwave::wmi_action_wql_query_msg> :
        binding::object<
            binding::inherit<traits<trustwave::action_msg> >,
        TAO_JSON_BIND_REQUIRED( "wql", &trustwave::wmi_action_wql_query_msg::wql ),
    TAO_JSON_BIND_OPTIONAL( "wmi_namespace", &trustwave::wmi_action_wql_query_msg::wmi_namespace ) >
{
    TAO_JSON_DEFAULT_KEY( trustwave::wmi_action_wql_query_msg::act_name.data() );


    template< template< typename... > class Traits >
    static trustwave::wmi_action_wql_query_msg as( const tao::json::basic_value< Traits >& v )
    {
        trustwave::wmi_action_wql_query_msg result;
        const auto o = v.at(trustwave::wmi_action_wql_query_msg::act_name);
        result.id_ = o.at( "id" ).template as< std::string >();
        result.wql = o.at( "wql" ).template as< std::string >();
        result.wmi_namespace = o.at( "wmi_namespace" ).template as< std::string >();

        return result;
    }

};


} // end of namespace tao::json

class session;
class result_msg;
namespace trustwave {

    class WMI_WQL_Action : public Action_Base {

    public:
        WMI_WQL_Action() : Action_Base(wmi_action_wql_query_msg::act_name){}

        action_status act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> action, std::shared_ptr<result_msg> res) override;
        [[nodiscard]] std::shared_ptr<action_msg> get_message(const tao::json::value& v) const override
        {
            return v.as<std::shared_ptr<trustwave::wmi_action_wql_query_msg>>();
        }
    };

} //end namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_ACTIONS_WQL_HPP