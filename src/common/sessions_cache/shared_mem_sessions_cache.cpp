//===========================================================================
// Trustwave ltd. @{SRCH}
//								shared_mem_sessions_cache.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 15 Jul 2019
// Comments:

#include "shared_mem_sessions_cache.hpp"
#include "../session.hpp"
#include "../singleton_runner/authenticated_scan_server.hpp"

#include <boost/smart_ptr/make_shared.hpp>

using namespace trustwave;
boost::shared_ptr<shared_mem_sessions_cache>
shared_mem_sessions_cache::get_or_create(const std::string& name, const size_t size, size_t session_idle_time)
{
    // std::cout << "Creating Shared Mem Sessions Cache ( " << absolute_name << " ) size ( " << size << " )" <<
    // std::endl;
    boost::shared_ptr<shared_mem_sessions_cache> cache
        = boost::shared_ptr<shared_mem_sessions_cache>(new shared_mem_sessions_cache(name, size, session_idle_time));
    if(!cache->construct()) {
        //    std::cout << "Failed Shared Mem Sessions Cache name ( " << absolute_name << " ) size ( " << size  << " )"
        //    << std::endl;
        return boost::shared_ptr<shared_mem_sessions_cache>();
    }

    return cache;
}

shared_mem_session_element::shared_mem_session_element(sp_session_t sp_session, size_t idle_timeout,
                                                       const void_allocator& va):
    expiration_time_(chr::system_clock::to_time_t(chr::system_clock::now()) + idle_timeout),
    session_(session_converter::convert(sp_session, va))
{
}

std::ostream& trustwave::operator<<(std::ostream& os, const shared_mem_session_element& e)
{
     os << "ID           : " << e.s_id().c_str() << "\n"
              << "IP           : " << e.s_remote().c_str() << "\n"
              << "Expires at   : " << e.expiration_time_ << "\n";
    for(const auto& c:e.session().creds_)
    {
        os <<c.first<<":"<<c.second<<std::endl;
    }
    return os;
}

shared_mem_sessions_cache::shared_mem_sessions_cache(const std::string& name, const size_t size, size_t idle_timeout):
    map_(), name_(name), lock_(bip::open_or_create, (name + std::string("_lock")).c_str()), segment_size_(size),
    session_idle_timeout_(idle_timeout)
{
}

shared_mem_sessions_cache::~shared_mem_sessions_cache() = default;
bool shared_mem_sessions_cache::construct()
{
    try {
        WriteLock auto_lock(lock_);
        // Shared memory front-end that is able to construct objects
        // associated with a c-string.
        segment_ = boost::make_shared<bip::managed_shared_memory>(bip::open_or_create, name_.c_str(), segment_size_);
        if(!segment_) {
            //        printf("Failed Creating managed_shared_memory as owner");
            return false;
        }
        map_ = segment_->find_or_construct<sessions>((name_ + std::string("_SET")).c_str())(
            sessions::ctor_args_list(), segment_->get_allocator<shared_mem_session_element>());

        if(nullptr == map_) {
            //     printf("Failed to create shared map");
            return false;
        }
    }
    catch(bip::interprocess_exception& ex) {
        //     printf("Catched IPC exception : %s", ex.what());
        return false;
    }
    return true;
}

bool shared_mem_sessions_cache::add(const sp_session_t sp_session)
{
    // Instantiate shared memory STL-compatible allocator
    static const void_allocator va(segment_->get_segment_manager());
    try {
        WriteLock auto_lock(lock_);
        auto& remote_idx = map_->get<remote>();
        auto f = remote_idx.find(String(sp_session->remote().c_str(), char_allocator(segment_->get_segment_manager())));
        if(f != remote_idx.end()) {
            remote_idx.erase(f);
            //       printf("found in cache %s", sp_session->remote().c_str());
        }
        auto it = map_->insert(shared_mem_session_element(sp_session, session_idle_timeout_, va));
        if(!it.second) {
            //     printf("No need to insert duplicated Session ( ID: %s ) to cache",
            //                      sp_session->idstr().c_str());
            return true;
        }
        //     std::cerr << *it.first;
    }
    catch(...) {
        //  printf("Cannot allocate Session for cache");
        return false;
    }

    //  printf("Session ( ID: %s ) added \n", sp_session->idstr().c_str());
    return true;
}

bool shared_mem_sessions_cache::remove_by_id(const std::string& ids)
{
    WriteLock auto_lock(lock_);
    auto& id_idx = map_->get<id>();
    auto s = id_idx.find(String(ids.c_str(), char_allocator(segment_->get_segment_manager())));
    if(s == id_idx.end()) {
        return false;
    }
    else {
        //      std::cerr << "dest: " << s->session_.remote_.c_str() << '\n';
        id_idx.erase(s);
        return true;
    }
}

bool shared_mem_sessions_cache::clean()
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto& exp_idx = map_->get<expiration>();
    decltype(exp_idx.find(String{})) till_now;
    {
        ReadLock auto_lock(lock_);
        till_now = exp_idx.lower_bound(now);
    }
    WriteLock auto_lock(lock_);

    for(auto it = exp_idx.begin(); it != till_now; ++it) {
        //     std::cerr << "dest: " << it->session_.remote_.c_str() << '\n';
        //      std::cerr << "expiration: " << it->expiration_time_ << " "
        //                      << (it->expiration_time_ <= now ? std::string(" expired") : std::string(" valid ")) <<
        //                      "\n";
    }
    exp_idx.erase(exp_idx.begin(), till_now);
    return true;
}

// Clear all entries
void shared_mem_sessions_cache::flush_all_entries()
{
    WriteLock auto_lock(lock_);
    map_->clear();
}

std::ostream& trustwave::operator<<(std::ostream& os, const shared_mem_sessions_cache& cache)
{
    shared_mem_sessions_cache::ReadLock auto_lock(cache.lock_);

    os << '\n'
       << "=========================================================================\n"
       << "=============================== Sessions Cache ==========================\n"
       << "=========================================================================\n"
       << "\n"
       << "Address:           " << cache.map_ << "\n"
       << "Name:              " << cache.name_ << "\n"
       << "Size:              " << cache.segment_size_ << "\n"
       << "Free size:         " << cache.segment_->get_free_memory() << "\n"
       << "Number of entries: " << cache.map_->size() << "\n"
       << "\n";

    auto end = cache.map_->end();
    for(auto it = cache.map_->begin(); it != end; ++it) {
        os << *it << '\n';
    }
    return os;
}

void shared_mem_sessions_cache::dump_by_expiration() const
{
    ReadLock auto_lock(lock_);
    auto now = chr::system_clock::to_time_t(chr::system_clock::now());
    std::cerr << '\n'
              << "=============================== Sessions Cache ==========================\n"
              << "\n"
              << "Number of entries: " << map_->size() << "\n"
              << "time_now: " << now << "\n";

    auto end = map_->get<expiration>().end();
    for(auto it = map_->get<expiration>().begin(); it != end; ++it) {
        std::cerr << "dest: " << it->session_.remote_.c_str() << '\n';
        std::cerr << "expiration: " << it->expiration_time_ << " "
                  << (it->expiration_time_ <= now ? std::string(" expired") : std::string(" valid ")) << "\n";
    }
}
