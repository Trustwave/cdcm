//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wrapper.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/10/19
// Comments:
//=====================================================================================================================
//                          						Include
//                          files
//=====================================================================================================================
#ifndef TESTS_FUNCTIONAL_TESTS_BOOST_TEST_WRAPPER_WRAPPER_HPP
#define TESTS_FUNCTIONAL_TESTS_BOOST_TEST_WRAPPER_WRAPPER_HPP
#include <iostream>
#include <map>
#include <memory>
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wpessimizing-move"
#include <boost/process.hpp>
// restore compiler switches
#pragma GCC diagnostic pop
#include <boost/asio.hpp>
#include <utility>
namespace trustwave {
    class wrapper {
    public:
        explicit wrapper(boost::asio::io_service& ios);

        ~wrapper();

        std::unique_ptr<boost::process::child>
        start_external_test(std::string command, std::string params,
                            std::pair<std::future<std::string>, std::future<std::string>>&& streams,
                            std::future<std::error_code>&&);

    private:
        boost::asio::io_service& ios;
        std::map<std::string, std::unique_ptr<boost::process::child>> workers_pool;
    };

} // namespace trustwave
#endif // TESTS_FUNCTIONAL_TESTS_BOOST_TEST_WRAPPER_WRAPPER_HPP