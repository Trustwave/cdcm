//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														session_cache_test.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : ascohen
// Date    : 8/26/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "common/sessions_cache/shared_mem_sessions_cache.hpp"
#include "common/session.hpp"
namespace utf = boost::unit_test;
using  namespace trustwave;
struct EmptyCache {
    EmptyCache()  {


        sessions = shared_mem_sessions_cache::get_or_create("sessions_test", 1024 * 1024 * 10,5);
        sessions -> flush_all_entries();
    }
    ~EmptyCache() {  }
    boost::shared_ptr <shared_mem_sessions_cache> sessions;
};
struct OneInCache:EmptyCache {
    OneInCache()  {
        credentials creds("WORKGROUP","admin1", "pass1", "ws1");
        auto s=boost::make_shared<trustwave::session>("192.168.0.1", creds);
        sessions->add(s);
    }
    ~OneInCache() {  }

};
struct TwoInCache:OneInCache {
    TwoInCache()  {
        credentials creds("WORKGROUP2","admin2", "pass2", "ws2");
        auto s=boost::make_shared<trustwave::session>("192.168.0.2", creds);
        sessions->add(s);
    }
    ~TwoInCache() {  }

};


BOOST_AUTO_TEST_SUITE(Session_Cache)

    BOOST_FIXTURE_TEST_CASE(Add,EmptyCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(!f);
        credentials creds("WORKGROUP","admin1", "pass1", "ws1");
        auto s=boost::make_shared<trustwave::session>("192.168.0.1", creds);
        sessions->add(s);
        f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_TEST(f);

    }
    BOOST_FIXTURE_TEST_CASE(Remove_By_ID,TwoInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        sessions->remove_by_id(f->idstr());
        f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_TEST(!f);

    }
    BOOST_FIXTURE_TEST_CASE(clean,OneInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        sessions->clean();
        f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        sleep(6);
        sessions->clean();
        f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_TEST(!f);
    }
    BOOST_FIXTURE_TEST_CASE(Get_By_ID,OneInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        auto id = f->idstr();
        auto f2=sessions->get_session_by<shared_mem_sessions_cache::id>(id);
        BOOST_TEST_MESSAGE(f->idstr());
        BOOST_TEST_MESSAGE(f2->idstr());
        BOOST_TEST(f->idstr()==f2->idstr());

    }

    BOOST_FIXTURE_TEST_CASE(Get_By_Remote,OneInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_TEST(f);
    }
    BOOST_FIXTURE_TEST_CASE(Touch_By_ID,OneInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        sleep(2);
        sessions->touch_by<shared_mem_sessions_cache::id>(f->idstr());
        sleep(5);
        auto f2=sessions->get_session_by<shared_mem_sessions_cache::id>(f->idstr());
        BOOST_TEST(f2);
    }

    BOOST_FIXTURE_TEST_CASE(Touch_By_Remote,OneInCache) {
        auto f=sessions->get_session_by<shared_mem_sessions_cache::remote>("192.168.0.1");
        BOOST_REQUIRE(f);
        sleep(2);
        sessions->touch_by<shared_mem_sessions_cache::remote>(f->remote());
        sleep(5);
        auto f2=sessions->get_session_by<shared_mem_sessions_cache::remote>(f->remote());
        BOOST_TEST(f2);
    }


BOOST_AUTO_TEST_SUITE_END()
