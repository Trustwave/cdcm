//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														dispatcher.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 16 May 2019
// Comments:
#ifndef TRUSTWAVE_COMMON_DISPATCHER_HPP_
#define TRUSTWAVE_COMMON_DISPATCHER_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <map>
#include <string>
#include <memory>
#include <iostream>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
template<typename T>
class Dispatcher
{
protected:
public:
    Dispatcher()
    = default;
    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher &) = delete;
    Dispatcher(Dispatcher &&) = delete;
    Dispatcher & operator=(Dispatcher &&) = delete;

public:
    using T_Ptr = std::shared_ptr<T>;
    typedef std::map<std::string, T_Ptr> Ts_Map;

    virtual ~Dispatcher()
    = default;
    void register1(T *t)
    {
        map_[t->command()] = std::shared_ptr<T>(t);
    }
    T_Ptr find(const std::string &t_name) const
    {
        auto it = map_.find(t_name);

        if (it == map_.end()) {
            return std::shared_ptr<T>();
        }
        return it->second;
    }
    struct Registrator
    {
        Registrator(T *t, Dispatcher & d)
        {
            std::cerr<<t->name()<<std::endl;
            d.register1(t);
        }
        Registrator(T *t, Dispatcher * d)
        {
            d->register1(t);
        }
    };
protected:
    Ts_Map map_;
};

}

#endif /* TRUSTWAVE_COMMON_DISPATCHER_HPP_ */
