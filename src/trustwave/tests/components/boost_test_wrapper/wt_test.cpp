//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wt_test.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/11/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "wrapper.hpp"
using namespace trustwave;

struct fix1 {
    fix1():w(ios)  {

    }
    ~fix1() {  }
    boost::asio::io_service ios;
    std::pair<std::future<std::string>,std::future<std::string>> pa=std::make_pair(std::future<std::string>(),std::future<std::string>());
    wrapper w;
};

BOOST_FIXTURE_TEST_SUITE(Functional,fix1)

    BOOST_AUTO_TEST_CASE(abc) {
        auto p=w.start_external_test("ls","-la",std::move(pa));
        ios.run();
        std::cout<<pa.first.get();
        BOOST_TEST(true);
    }


BOOST_AUTO_TEST_SUITE_END()
