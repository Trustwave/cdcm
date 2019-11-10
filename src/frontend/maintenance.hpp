//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														maintenance.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 11/7/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_FRONTEND_MAINTENANCE_HPP
#define SRC_FRONTEND_MAINTENANCE_HPP

#include <boost/asio.hpp>
#include <chrono>
namespace trustwave {
    class maintenance
    {
    public:
        maintenance(boost::asio::io_service& ios);
        void maintenance_function();
    private:
        boost::asio::steady_timer timer_;
    };
}
#endif //SRC_FRONTEND_MAINTENANCE_HPP