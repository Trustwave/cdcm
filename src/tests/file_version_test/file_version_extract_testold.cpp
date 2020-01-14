//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_version_extract_test.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      :
// Date    : 9/15/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
extern "C" {
int get_ver(const char* path, char* res);
}
BOOST_AUTO_TEST_SUITE(Utils)

BOOST_AUTO_TEST_CASE(file_version)
{
    std::string res;
    res.resize(256, 0);
    std::string assets_path;

    BOOST_TEST_MESSAGE(boost::unit_test::framework::master_test_suite().argv[2]);

    if(boost::unit_test::framework::master_test_suite().argc < 3) {
        assets_path.assign("assets/pe_test.asset");
    }
    else {
        assets_path.assign(boost::unit_test::framework::master_test_suite().argv[2]).append("/pe_test.asset");
    }
    std::cerr << assets_path;
    BOOST_TEST_MESSAGE(assets_path);

    if(-1 == get_ver(assets_path.c_str(), std::addressof(res[0]))) {
        BOOST_TEST(false);
    }
    res.resize(strlen(res.c_str()));

    BOOST_TEST(res == "5.2.3790.1830");
}
BOOST_AUTO_TEST_SUITE_END()