//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_wql_client.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/13/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_WMI_WQL_CLIENT_HPP
#define SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_WMI_WQL_CLIENT_HPP
#include "client.hpp"
#include <string>
#include <tuple>
#include <boost/python.hpp>

namespace trustwave {

    class session; //forward declaration

    class wmi_wql_client final :public cdcm_client {

    public:
        using result = std::tuple<bool,std::string>;
        static constexpr std::string_view protocol{"wmi_wql"};

        wmi_wql_client() :cdcm_client(protocol){}
        ~wmi_wql_client()= default;

        result connect(const session& session, const std::string & wmi_namespace);
        result query_remote_asset(const std::string & wql_query);
        result close_connection();

    private:
        using bpo = boost::python::object;
        bpo main_;
        bpo global_;
        bpo helper_;
        bpo exec_;
    };


} //end namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_WMI_WQL_CLIENT_HPP