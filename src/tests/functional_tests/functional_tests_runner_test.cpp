//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														functional_tests_runner_test.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 9/19/19
// Comments:
//=====================================================================================================================
//                          						Include
//                          files
//=====================================================================================================================
#define BOOST_TEST_DYN_LINK
#include "../components/boost_test_wrapper/wrapper.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/iterator_range_io.hpp>
#include <boost/regex.hpp>
#include <boost/test/unit_test.hpp>
#include <string>
BOOST_AUTO_TEST_SUITE(Functional)

BOOST_AUTO_TEST_CASE(standard)
{
    boost::asio::io_service ios;
    trustwave::wrapper w(ios);
    std::future<std::error_code> e;
    auto pa = std::make_pair(std::future<std::string>(), std::future<std::string>());
    auto p = w.start_external_test("exe_test", "./functional_tests/actions.xml", std::move(pa), std::move(e));
    ios.run();
    auto out = pa.first.get();
    auto err = pa.second.get();
    auto ec = e.get();
    std::cerr << out << std::endl;
    std::cerr << err << std::endl;
    std::cerr << ec << std::endl;
    BOOST_TEST_REQUIRE(!ec, "failed to run wrapper");
    std::vector<std::string> cont;
    boost::split(cont, out, boost::is_any_of("\n"));
    BOOST_TEST_MESSAGE("Ran " << cont.size() << " tests");
    boost::regex re(".*:(.*) .*:(.*) .*:(.*) .*:(.*) .*:(.*) .*:\\[(.*)\\]");
    const int subs[] = {1};

    for(auto line: cont) {
        if(boost::starts_with(line, "result")) {
            enum {
                result = 1,
                ip,
                action_name,
                session_name,
                req_id,
                reason

            };
            boost::regex_token_iterator<std::string::iterator> it(line.begin(), line.end(), re, subs);
            boost::regex_token_iterator<std::string::iterator> end;
            boost::smatch what;
            if(boost::regex_search(line, what, re)) {
                BOOST_TEST(what[result] == "passed", "Test: '" << what[action_name] << "' of session: '"
                                                               << what[session_name] << "' with reason: '"
                                                               << what[reason] << "'");
            }
        }
    }
}
BOOST_AUTO_TEST_SUITE_END()