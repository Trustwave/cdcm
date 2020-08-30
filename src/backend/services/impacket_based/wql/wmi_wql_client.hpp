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
#include <boost/python.hpp>
#include "client.hpp"
#include "../common/typedefs.hpp"
#include "dispatcher.hpp"

namespace trustwave {

    class session; //forward declaration

    class wmi_wql_client final :public cdcm_client {
        static Dispatcher<cdcm_client>::Registrator m_registrator;

    public:
        static constexpr std::string_view protocol{"wmi_wql"};

        wmi_wql_client() :cdcm_client(protocol){}
        ~wmi_wql_client() = default;

        impacket_based_common::result connect(const session& session, const std::string & wmi_namespace);
        impacket_based_common::result query_remote_asset(const std::string & wql_query);
        impacket_based_common::result close_connection();

    private:
        using bpo = boost::python::object;
        bpo main_;
        bpo global_;
        bpo helper_;
        bpo exec_;
    };


} //end namespace trustwave
#endif // SRC_BACKEND_SERVICES_IMPACKET_BASED_WQL_WMI_WQL_CLIENT_HPP