/*
 * sessions_container.hpp
 *
 *  Created on: Apr 29, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_SESSIONS_CONTAINER_HPP_
#define TRUSTWAVE_MISC_SESSIONS_CONTAINER_HPP_
#include "session.hpp"
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
namespace trustwave {
//can't really avoid this singleton

class sessions_container
{
private:
public:
    sessions_container()
    {
    }
    sessions_container(const sessions_container&) = delete;
    sessions_container& operator=(const sessions_container &) = delete;
    sessions_container(sessions_container &&) = delete;
    sessions_container & operator=(sessions_container &&) = delete;


    struct element{
            mutable time_t first;
            session second;
            const boost::uuids::uuid & s_id() const
            {
                return second.id();
            }
            const std::string & s_remote() const
            {
                return second.remote();
            }
        };
    void insert_session(session s)
    {
        element e;
        e.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());;
        e.second= s;
        sessions_.insert(e);
    }
private:
    struct id
    {
    };
    struct remote
    {
    };
    struct service_type
    {
    };
    struct last_time
        {
        };

    typedef boost::multi_index::multi_index_container<
                    element,
                    boost::multi_index::indexed_by<
                        boost::multi_index::ordered_unique<
                            boost::multi_index::tag<id>,
                            boost::multi_index::const_mem_fun<element, const boost::uuids::uuid &, &element::s_id>
                        >,
                        boost::multi_index::ordered_non_unique<
                            boost::multi_index::tag<last_time>,
                            boost::multi_index::member<element,  time_t, &element::first>
                                                >,
                        boost::multi_index::ordered_unique<
                            boost::multi_index::tag<remote>,
                            boost::multi_index::const_mem_fun<element, const std::string &, &element::s_remote>
                        >
                    >
    > sessions;

    sessions sessions_;
    session return_replaced(sessions::const_iterator s)
    {
        typedef boost::multi_index::index<sessions, last_time>::type element_by_time;
        element_by_time& time_index = sessions_.get<last_time>();
        auto it2 = sessions_.project<last_time>(s);
        auto n = *s;
        n.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        time_index.replace(it2, n);
        return s->second;
    }

public:
    session get_session_by_dest(const std::string dst)
    {
        auto s = sessions_.get<remote>().find(dst);
        if (s == sessions_.get<remote>().end()) {
            return session();
        }
        else {

            typedef boost::multi_index::index<sessions, last_time>::type element_by_time;
                    element_by_time& time_index = sessions_.get<last_time>();
                    auto it2 = sessions_.project<last_time>(s);
                    auto n = *s;
                    n.first = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    time_index.replace(it2, n);
                    return s->second;
        }
//  //  boost::multi_index::get<remote>(sessions_).find(dst);
//  return  sessions_.get<remote>().find(dst);
    }
    session get_session_by_id(const std::string ids)
    {
        boost::uuids::uuid uuid_id;
        try {
            uuid_id = boost::uuids::string_generator()(ids);
        } catch (std::runtime_error& ex) {
            return session();
        }
        auto s = sessions_.get<id>().find(uuid_id);
        if (s == sessions_.get<id>().end()) {
            return session();
        }
        else {
            return return_replaced(s);
        }

    }
    void dump_by_time() const
    {
//        std::cerr << std::endl;
//        for (const auto s : sessions_.get<last_time>()) {
//            std::cerr << s.second.remote() << " " << s.first << std::endl;
//        }
    }
};
}

#endif /* TRUSTWAVE_MISC_SESSIONS_CONTAINER_HPP_ */
