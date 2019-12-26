//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														maintenance.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/10/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "maintenance.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include <boost/filesystem.hpp>
#include <boost/uuid/random_generator.hpp>  // for random_generator
#include <boost/uuid/uuid_io.hpp>           // for to_string
using namespace trustwave;
maintenance::maintenance(boost::asio::io_service& ios):timer_(ios, authenticated_scan_server::instance().settings.maintenance_interval_)
{
timer_.async_wait(boost::bind(&maintenance::maintenance_function,
                              this));
}

void maintenance::maintenance_function()
{
    static const auto uuid_str_len = boost::uuids::to_string(boost::uuids::random_generator()()).length();
    authenticated_scan_server::instance().sessions->clean();
    auto session_list = authenticated_scan_server::instance().sessions->get_sessions_id_list();
    for (const auto & entry : boost::filesystem::directory_iterator(authenticated_scan_server::instance().settings.downloaded_files_path_))
    {
        if ( session_list.end() == std::find(session_list.begin(),session_list.end(),std::string(entry.path().filename().string().c_str(),uuid_str_len)))
        {
            boost::filesystem::remove(entry.path());
        }
    }

    timer_.expires_at(timer_.expiry() +authenticated_scan_server::instance().settings.maintenance_interval_);
    timer_.async_wait(boost::bind(&maintenance::maintenance_function, this));
}