//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														worker_container_test.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : ascohen
// Date    : 8/27/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <string>
#include <chrono>                                            // for system_clock, chrono
#include "frontend/worker_container.hpp"
using namespace trustwave;

struct EmptyContainer {
    EmptyContainer():workers(std::chrono::seconds(5))  {
    }
    ~EmptyContainer() {  }
    worker_container workers;
};
struct OneInContainer:public EmptyContainer
{
    OneInContainer()
    {
        workers.insert(std::make_shared<worker>("worker1"));
    }

};
struct OneBusyInContainer:public EmptyContainer
{
    OneBusyInContainer()
    {
        auto w = std::make_shared<worker>("worker1");
        w->idle_=false;
        workers.insert(w);
    }

};
struct TwoInContainer:public OneInContainer
{
    TwoInContainer()
    {
        workers.insert(std::make_shared<worker>("worker2"));
    }

};
struct TenInContainer:public EmptyContainer
{
    TenInContainer()
    {
        for(size_t i=1;i<=10;++i)
        {
            workers.insert(std::make_shared<worker>((std::string("worker")+std::to_string(i))));
        }
        result.push_back("worker1");
        result.push_back("worker3");
        result.push_back("worker4");
        result.push_back("worker6");
        result.push_back("worker7");
        result.push_back("worker8");
        result.push_back("worker10");

    }
    std::vector<std::string> result;

};
BOOST_AUTO_TEST_SUITE(Worker_Container)

    BOOST_FIXTURE_TEST_CASE(insert,EmptyContainer) {
        workers.insert(std::make_shared<worker>("worker"));
        auto f = workers.get(std::string("worker"));
        BOOST_TEST(f);
        }
    BOOST_FIXTURE_TEST_CASE(exists,OneInContainer) {

        BOOST_TEST(workers.exists(std::string("worker1")));
    }
BOOST_FIXTURE_TEST_CASE(get,OneInContainer) {
        auto f = workers.get(std::string("worker1"));

        BOOST_TEST(f->identity_=="worker1");
    }
    BOOST_FIXTURE_TEST_CASE(get_next_worker,TwoInContainer) {
        auto f = workers.get_next_worker();

        BOOST_TEST(f->identity_=="worker2");
    }
    BOOST_FIXTURE_TEST_CASE(update_last_worked,TwoInContainer) {
        auto f = workers.get_next_worker();
        workers.update_last_worked(f->identity_,"session2");
        f=workers.get(f->identity_);
        BOOST_TEST(f->last_worked_session_=="session2");
    }

    BOOST_FIXTURE_TEST_CASE(get_by_last_worked_session,TwoInContainer) {
        auto f = workers.get_next_worker();
        workers.update_last_worked(f->identity_,"session2");
        f=workers.get_by_last_worked_session("session2");
        BOOST_TEST(f->identity_=="worker2");
    }


    BOOST_FIXTURE_TEST_CASE(erase_not_exists,EmptyContainer) {
        BOOST_TEST(!workers.erase("worker"));
        }
    BOOST_FIXTURE_TEST_CASE(erase_exists_ret_val,OneInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_TEST(workers.erase("worker1"));
    }
    BOOST_FIXTURE_TEST_CASE(erase_exists,OneInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        workers.erase("worker1");

        BOOST_TEST(!workers.exists("worker1"));
    }
BOOST_FIXTURE_TEST_CASE(is_idle_true,OneInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_TEST(workers.is_idle("worker1"));
    }
    BOOST_FIXTURE_TEST_CASE(is_idle_false,OneBusyInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_TEST(!workers.is_idle("worker1"));
    }
    BOOST_FIXTURE_TEST_CASE(is_busy_true,OneBusyInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_TEST(workers.is_busy("worker1"));
    }
    BOOST_FIXTURE_TEST_CASE(is_busy_false,OneInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_TEST(!workers.is_busy("worker1"));
    }
BOOST_FIXTURE_TEST_CASE(set_idle,OneBusyInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_REQUIRE(workers.is_busy("worker1"));
        workers.set_idle("worker1");
        BOOST_TEST(workers.is_idle("worker1"));
    }
BOOST_FIXTURE_TEST_CASE(set_busy,OneInContainer) {
        BOOST_REQUIRE(workers.exists("worker1"));
        BOOST_REQUIRE(workers.is_idle("worker1"));
        workers.set_busy("worker1");
        BOOST_TEST(workers.is_busy("worker1"));
    }

BOOST_FIXTURE_TEST_CASE(idle_workers,TenInContainer) {
        workers.set_busy("worker2");
        workers.set_busy("worker5");
        workers.set_busy("worker9");
        std::vector<std::string> idles;
        auto p = workers.idle_workers();
        for(auto it= p.first;it != p.second;++it ) {
            idles.push_back((*it)->identity_);
        }
        BOOST_TEST( idles==result,boost::test_tools::per_element());


    }
//    typedef std::chrono::time_point<std::chrono::system_clock> exp_type;
//    BOOST_TEST_DONT_PRINT_LOG_VALUE(exp_type)
BOOST_FIXTURE_TEST_CASE(update_expiration,OneInContainer) {
        auto f = workers.get(std::string("worker1"));
        BOOST_REQUIRE(f);
        auto exp = f->expiry_;
        workers.update_expiration(f->identity_);
        f = workers.get(std::string("worker1"));
        bool rv = exp<f->expiry_;
        BOOST_TEST(true ==rv );
    }
BOOST_AUTO_TEST_SUITE_END()
