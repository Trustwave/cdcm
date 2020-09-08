//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														supervisor.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 2/26/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_SUPERVISOR_SUPERVISOR_HPP
#define SRC_SUPERVISOR_SUPERVISOR_HPP
#include <boost/asio.hpp>
#include "zmq/zmq_helpers.hpp"
#include "workers_monitor.hpp"

namespace trustwave {
    class supervisor final {
    public:
        explicit supervisor(boost::asio::io_service& ios): ios_(ios), worker_monitor_(ios) {}
        void run()
        {
            broker_ = start_broker();
            worker_monitor_.run();
        }

    private:
        std::unique_ptr<boost::process::child> start_broker()
        {
            try {
                auto broker = std::make_unique<boost::process::child>(
                    boost::process::search_path("cdcm_broker"),
                    boost::process::on_exit([&, this](int, const std::error_code&) {
                        if(!trustwave::zmq_helpers::interrupted) {
                            broker_ = start_broker();
                        }
                    }),
                    ios_);
                return broker;
            }
            catch(std::exception& exception) {
                AU_LOG_ERROR("got exception while trying to start broker. exception: %s", exception.what());
                return nullptr;
            }
        }
        boost::asio::io_service& ios_;
        workers_monitor worker_monitor_;
        std::unique_ptr<boost::process::child> broker_;
    };
} // namespace trustwave
#endif // SRC_SUPERVISOR_SUPERVISOR_HPP