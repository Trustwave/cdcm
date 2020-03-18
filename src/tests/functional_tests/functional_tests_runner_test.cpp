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
    std::future<std::error_code> e_cdcm;
    // get a handle to the current environment
    auto env = boost::this_process::environment();
    auto ld_path = std::string("/opt/output/") + env["CI_COMMIT_BRANCH"].to_string() + std::string("/libs");
    constexpr std::string_view samba_dir = "deps/samba-4.10.6/bin/shared";
    auto samba_path = boost::filesystem::current_path().parent_path().parent_path().append(
        samba_dir); // ../../deps/samba-4.10.6/b in/shared

    env["LD_LIBRARY_PATH"] += ld_path;
    env["LD_LIBRARY_PATH"] += samba_path.string();
    env["LD_LIBRARY_PATH"] += samba_path.append("private").string();
    BOOST_TEST_MESSAGE("LD_LIBRARY_PATH: " << env["LD_LIBRARY_PATH"].to_string());
    boost::process::system("pkill cdcm", env);
    const char* sed1 = R"foo(sed -i s/\/usr\/share\/cdcm\/lib\/plugins/)foo";
    const char* sed2 = R"foo(/g /etc/cdcm/cdcm_settings.json)foo";
    auto ld_path1 = R"foo(\/opt\/output\/)foo";
    auto ld_path2 = env["CI_COMMIT_BRANCH"].to_string();
    auto ld_path3 = R"foo(\/libs\/plugins)foo";
    auto sed_cmd = std::string(sed1) + ld_path1 + ld_path2 + ld_path3 + sed2;
    std::cerr<< sed_cmd << std::endl;
    boost::process::system(sed_cmd, env);
    boost::process::spawn("cdcm_supervisor", env);
    std::future<std::error_code> e;
    auto pa = std::make_pair(std::future<std::string>(), std::future<std::string>());
    auto p = w.start_external_test("exe_test", "full.xml", std::move(pa), std::move(e));
    ios.run();
    auto out = pa.first.get();
    auto err = pa.second.get();
    auto ec = e.get();
    BOOST_TEST_MESSAGE("out " << out);
    BOOST_TEST_MESSAGE("err " << err);

    boost::process::system("pkill cdcm", env);
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