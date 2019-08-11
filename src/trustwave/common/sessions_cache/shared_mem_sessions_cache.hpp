//===========================================================================
// Trustwave ltd. @{SRCH}
//								shared_mem_sessions_cache.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 14 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_SESSIONS_CACHE_HPP_
#define TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_SESSIONS_CACHE_HPP_
#include "shared_mem_converters.hpp"
#include "shared_mem_session.hpp"
#include <boost/interprocess/interprocess_fwd.hpp>           // for interpro...
#include <boost/interprocess/sync/named_sharable_mutex.hpp>  // for named_sh...
#include <boost/interprocess/sync/scoped_lock.hpp>           // for scoped_lock
#include <boost/interprocess/sync/sharable_lock.hpp>           // for sharable_lock
#include <boost/multi_index_container.hpp>           // for interpro...
#include <boost/multi_index/ordered_index.hpp>           //
#include <boost/multi_index/mem_fun.hpp>                      // for multi_index
#include <boost/multi_index/member.hpp>                      // for multi_index
#include <boost/multi_index/tag.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>                    // for shared_ptr
#include <iosfwd>                                            // for ostream
#include <string>                                            // for string
#include <chrono>
#include <type_traits>

namespace trustwave {
//===================================================================
//                      Forward declarations
//===================================================================
class session;
class shared_mem_sessions_cache;
struct shared_mem_session_element;
namespace bip = boost::interprocess;
namespace bmi = boost::multi_index;
namespace chr = std::chrono;

using sp_session_t = boost::shared_ptr<session>;
using void_allocator = bip::managed_shared_memory::allocator<void>::type;
using char_allocator = bip::managed_shared_memory::allocator<char>::type;
typedef bip::basic_string <char, std::char_traits <char>, char_allocator> String;

std::ostream &operator<<(std::ostream &os, const shared_mem_session_element&);
std::ostream &operator<<(std::ostream &os, const shared_mem_sessions_cache&);

class shared_mem_session_element {
    friend std::ostream& ::trustwave::operator<<(std::ostream &os,
                    const trustwave::shared_mem_session_element&ident_entry);
    friend class shared_mem_sessions_cache;
public:
    String s_id() const
    {
        return session_.uuid_;
    }
    String s_remote() const
    {
        return session_.remote_;
    }
    ~shared_mem_session_element()
    {
        printf("%s is being destructed\n", session_.remote_.c_str());
    }
    shared_mem_session_element(sp_session_t spUC, size_t, const void_allocator &va);
    // Avoid copy semantic
    shared_mem_session_element(const shared_mem_session_element&) = delete;
    shared_mem_session_element& operator=(const shared_mem_session_element&) = delete;

    // Force move semantic
    shared_mem_session_element(shared_mem_session_element&&) = default;
    shared_mem_session_element& operator=(shared_mem_session_element&&) = default;

    void dump() const;
    time_t expiration_time() const
    {
        return expiration_time_;
    }

    const shared_mem_session& session() const
    {
        return session_;
    }
private:
    mutable time_t expiration_time_;
    shared_mem_session session_;

};
class shared_mem_sessions_cache {
public:
    //Tags
    struct id;
    struct remote;
private:
    struct expiration;
    friend std::ostream &::trustwave::operator<<(std::ostream &os, const trustwave::shared_mem_sessions_cache&cache);
    using ReadLock = bip::sharable_lock<bip::named_sharable_mutex>;
    using WriteLock = bip::scoped_lock<bip::named_sharable_mutex>;

    typedef bmi::multi_index_container <shared_mem_session_element,
                    bmi::indexed_by <
                                    bmi::ordered_unique <boost::multi_index::tag <id>,
                                                    bmi::const_mem_fun <shared_mem_session_element, String,
                                                                    &shared_mem_session_element::s_id> >,
                                    bmi::ordered_non_unique <bmi::tag <expiration>,
                                                    bmi::const_mem_fun <shared_mem_session_element, time_t,
                                                                    &shared_mem_session_element::expiration_time> >,
                                    bmi::ordered_unique <boost::multi_index::tag <remote>,
                                                    bmi::const_mem_fun <shared_mem_session_element, String,
                                                                    &shared_mem_session_element::s_remote> > >,
                    typename bip::managed_shared_memory::allocator <shared_mem_session_element>::type> sessions;

private:

    shared_mem_sessions_cache(const std::string &name, const size_t size, size_t);

    bool construct();
    template<typename Tag>
    sp_session_t update(decltype(::std::declval<typename sessions::index<Tag>::type>().find(String{})) s)
    {
        WriteLock auto_lock(lock_);
        auto it2 = map_->project <expiration>(s);
        map_->get <expiration>().modify(it2,
                        [this](shared_mem_session_element& x){x.expiration_time_=chr::system_clock::to_time_t(chr::system_clock::now())+session_idle_timeout_;});
        return session_converter::convert(s->session_);
    }

public:
    ~shared_mem_sessions_cache();

    // creation interface
public:
    static boost::shared_ptr <shared_mem_sessions_cache> get_or_create(const std::string &name, const size_t size);
public:
    bool add(sp_session_t);
    template<typename Tag>
    sp_session_t get_session_by(const std::string &kv)
    {
        auto& idx = map_->get <Tag>();
        decltype(idx.find(String{})) s;
        {
            ReadLock auto_lock(lock_);
            s = idx.find(String(kv.c_str(), char_allocator(segment_->get_segment_manager())));
        }
        if (s == idx.end()){
            printf("Failed finding Entry with ID ( ID: %s )", kv.c_str());
            return sp_session_t();
        }
        return this->update <Tag>(s);
    }
    template<typename Tag>
    bool touch_by(const std::string &kv)
    {
        auto& idx = map_->get <Tag>();
        decltype(idx.find(String{})) s;
        {
            ReadLock auto_lock(lock_);
            s = idx.find(String(kv.c_str(), char_allocator(segment_->get_segment_manager())));
        }
        if (s == idx.end()){
            printf("Failed finding Entry with ID ( ID: %s )", kv.c_str());
            return false;
        }
        this->update <Tag>(s);
        return true;
    }
    bool remove_by_id(const std::string &id);
    bool clean();
    void flush_all_entries();
    void dump_by_expiration() const;

private:
    sessions* map_;
    std::string name_;
    // mutable because of the need of lock in const member methods
    mutable bip::named_sharable_mutex lock_;
    boost::shared_ptr <bip::managed_shared_memory> segment_;
    size_t segment_size_;
    size_t session_idle_timeout_;

};

}

//

#endif /* TRUSTWAVE_COMMON_SESSIONS_CACHE_SHARED_MEM_SESSIONS_CACHE_HPP_ */
