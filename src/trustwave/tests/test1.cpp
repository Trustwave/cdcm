//
// Created by Assaf Cohen on 2019-08-19.
//

#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MAIN - don't need to repeat this define in more than one cpp file
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(CDCM_Suite)
BOOST_AUTO_TEST_CASE(IncorrectDate)
        {
        BOOST_REQUIRE_EQUAL(0, 1);
        }
BOOST_AUTO_TEST_SUITE_END()