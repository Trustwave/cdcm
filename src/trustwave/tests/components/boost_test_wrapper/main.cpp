//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														main.cpp
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

int main(int ,char**)
{
    boost::asio::io_service ios;
    trustwave::wrapper w(ios);
    auto pa = std::make_pair(std::future<std::string>(),std::future<std::string>());
    auto p=w.start_external_test("exe_test","../../functional_tests/actions.xml",std::move(pa));
    //auto p=w.start_external_test("pwd","",std::move(pa));
    ios.run();
    std::cout<<pa.first.get();
    std::cout<<pa.second.get();

    return 0;
}