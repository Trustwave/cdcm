//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														in_sessions_container.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 19 Apr 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_SESSIONS_CACHE_INPROC_SESSIONS_CACHE_HPP_
#define TRUSTWAVE_COMMON_SESSIONS_CACHE_INPROC_SESSIONS_CACHE_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <boost/multi_index_container.hpp>
#include <string>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <chrono>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <iostream>
#include <mutex>
#include "shared_mem_sessions_cache.hpp"
#include "../../common/session.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
template<typename Session_T>
class inproc_in_sessions_cache
{
private:
public:
    inproc_in_sessions_cache()
    {
        out_sessions_ = shared_mem_in_sessions_cache::get_or_create("sessions",1024*1024*10);
    }
    inproc_in_sessions_cache(const inproc_in_sessions_cache&) = delete;
    inproc_in_sessions_cache& operator=(const inproc_in_sessions_cache &) = delete;
    inproc_in_sessions_cache(inproc_in_sessions_cache &&) = delete;
    inproc_in_sessions_cache & operator=(inproc_in_sessions_cache &&) = delete;

    struct element
    {
        mutable time_t first;
        Session_T second;
        const boost::uuids::uuid & s_id() const
        {
            return second.id();
        }
        const std::string & s_remote() const
        {
            return second.remote();
        }
    };
    void insert_session(Session_T s)
    {
        auto f = in_sessions_.get<remote>().find(s.remote());
        if (f != in_sessions_.get<remote>().end()) {
            in_sessions_.get<remote>().erase(f);
        }
        element e;
        e.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        e.second = s;
        in_sessions_.insert(e);
    }
    struct id
    {
    };
    struct remote
    {
    };
    struct last_time
    {
    };
private:

    typedef boost::multi_index::multi_index_container<element,
                    boost::multi_index::indexed_by<
                                    boost::multi_index::ordered_unique<boost::multi_index::tag<id>,
                                                    boost::multi_index::const_mem_fun<element,
                                                                    const boost::uuids::uuid &, &element::s_id> >,
                                    boost::multi_index::ordered_non_unique<boost::multi_index::tag<last_time>,
                                                    boost::multi_index::member<element, time_t, &element::first> >,
                                    boost::multi_index::ordered_unique<boost::multi_index::tag<remote>,
                                                    boost::multi_index::const_mem_fun<element, const std::string &,
                                                                    &element::s_remote> > > > sessions;

    sessions in_sessions_;
    shared_mem_sessions_cache out_sessions_;
    std::mutex lock_;
    session return_replaced(sessions::const_iterator s)
    {
        typedef boost::multi_index::index<sessions, last_time>::type element_by_time;
        element_by_time& time_index = in_sessions_.get<last_time>();
        auto it2 = in_sessions_.project<last_time>(s);
        auto n = *s;
        n.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        lock_.lock();
        time_index.replace(it2, n);
        lock_.unlock();
        return s->second;
    }

public:
    session get_session_by_dest(const std::string dst)
    {
        auto s = in_sessions_.get<remote>().find(dst);
        if (s == in_sessions_.get<remote>().end()) {
            return session();
        }
        else {
            typedef boost::multi_index::index<sessions, last_time>::type element_by_time;
            element_by_time& time_index = in_sessions_.get<last_time>();
            auto it2 = in_sessions_.project<last_time>(s);
            auto n = *s;
            n.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            lock_.lock();
            time_index.replace(it2, n);
            lock_.unlock();
            return s->second;
        }

    }
    session get_session_by_id(const std::string ids)
    {
        boost::uuids::uuid uuid_id;
        try {
            uuid_id = boost::uuids::string_generator()(ids);
        } catch (std::runtime_error& ex) {
            return session();
        }
        auto s = in_sessions_.get<id>().find(uuid_id);
        if (s == in_sessions_.get<id>().end()) {
            return session();
        }
        else {
            return return_replaced(s);
        }

    }
    bool remove_by_id(const std::string ids)
    {
        boost::uuids::uuid uuid_id;
        try {
            uuid_id = boost::uuids::string_generator()(ids);
        } catch (std::runtime_error& ex) {
            return false;
        }
        auto s = in_sessions_.get<id>().find(uuid_id);
        if (s == in_sessions_.get<id>().end()) {
            return false;
        }
        else {
            lock_.lock();
            in_sessions_.get<id>().erase(s);
            lock_.unlock();
            return true;
        }

    }
    void dump_by_time() const
    {
        std::cerr << std::endl;
        for (const auto s : in_sessions_.get<last_time>()) {
            std::cerr << s.second.remote() << " " << s.second.idstr() << " " << s.first << std::endl;
        }
    }
};
}

#endif /* TRUSTWAVE_COMMON_SESSIONS_CACHE_INPROC_SESSIONS_CACHE_HPP_ */
