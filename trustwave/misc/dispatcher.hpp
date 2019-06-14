/*
 * dispatcher.hpp
 *
 *  Created on: May 16, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_DISPATCHER_HPP_
#define TRUSTWAVE_MISC_DISPATCHER_HPP_
#include <map>
#include <string>
#include <memory>
#include <iostream>
namespace trustwave {
template<typename T>
class Dispatcher
{
protected:
public:
    Dispatcher(){}
    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher &) = delete;
    Dispatcher(Dispatcher &&) = delete;
    Dispatcher & operator=(Dispatcher &&) = delete;

public:
    typedef std::shared_ptr<T> T_Ptr;
    typedef std::map<std::string, T_Ptr> Ts_Map;

    virtual ~Dispatcher()
    {
    }
    void register1(T *t)
    {
        map_[t->command()] = std::shared_ptr<T>(t);
    }
    T_Ptr find(const std::string &t_name) const
    {
        typename Ts_Map::const_iterator it = map_.find(t_name);

        if (it == map_.end()) {
            return std::shared_ptr<T>();
        }
        return it->second;
    }
    struct Registrator
    {
        Registrator(T *t, Dispatcher & d)
        {
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

#endif /* TRUSTWAVE_MISC_DISPATCHER_HPP_ */
