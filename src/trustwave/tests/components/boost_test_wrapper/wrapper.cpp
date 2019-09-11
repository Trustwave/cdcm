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
std::unique_ptr<boost::process::child> wrapper::start_external_test(std::string command,std::string params,std::pair<std::future<std::string>,std::future<std::string>>&& pa)
{
    try
    {

        auto p = std::make_unique<bp::child>( bp::search_path(command), params,
                                                              bp::on_exit( [  command, this ](int status, const std::error_code& ec) {
                                                                  std::cout << "on_exit handler called for worker: " << command << std::endl;
                                                                  std::cout << "on_exit status value: " << status << std::endl;
                                                                  std::cout << "on_exit error code value: " << ec.value() << std::endl;
                                                                  std::cout << "on_exit error code value: " << ec.message() << std::endl;
                                                                  std::cout << "on_exit error code category: " << ec.category().name() << std::endl;

                                                              }), bp::std_in.close(),bp::std_out > pa.first, bp::std_err >pa.second,ios) ;

        return std::move(p);
    }
    catch (std::exception& exception)
    {
        std::cout << "got exception: " << exception.what() << std::endl;
        return nullptr;
    }
}