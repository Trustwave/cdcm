//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wrapper.cpp
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
//                          						Include files
//=====================================================================================================================
#include "wrapper.hpp"
using namespace trustwave;
namespace bp=boost::process;
wrapper::wrapper(boost::asio::io_service& ios_) : ios(ios_)
{
}

wrapper::~wrapper()
{
    for (auto iter = workers_pool.begin(); iter != workers_pool.end() ; ++iter )
    {
        iter->second->terminate();
    }
    workers_pool.erase(workers_pool.begin(), workers_pool.end());
}
std::unique_ptr<boost::process::child> wrapper::start_external_test(std::string command,std::string params,std::pair<std::future<std::string>,std::future<std::string>>&& streams,std::future<std::error_code>&& ec_ret)
{
    const std::vector< boost::filesystem::path > cp={boost::filesystem::path("./"),
                                                     boost::filesystem::path("../"),
                                                     boost::filesystem::path("./functional_tests")};
    try
    {

        std::shared_ptr<std::promise<std::error_code>> promise = std::make_shared<std::promise<std::error_code>>();
        ec_ret= promise->get_future();
        auto p = std::make_unique<bp::child>(bp::search_path(command,cp), params,
                                             bp::on_exit( [  command, this ,promise](int , const std::error_code& ec) {
                                                 promise->set_value(std::move(ec));


                                                              }), bp::std_in.close(), bp::std_out > streams.first, bp::std_err > streams.second, ios) ;

        return std::move(p);
    }
    catch (std::exception& exception)
    {
        std::cout << std::endl<< "Got exception: " << exception.what() << std::endl;
        return nullptr;
    }
}