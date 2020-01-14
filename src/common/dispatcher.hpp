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
#include <iostream>
#include <map>
#include <memory>
#include <string>
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {
    template<typename T> class Dispatcher final {
    protected:
    public:
        Dispatcher() = default;
        Dispatcher(const Dispatcher&) = delete;
        Dispatcher& operator=(const Dispatcher&) = delete;
        Dispatcher(Dispatcher&&) = delete;
        Dispatcher& operator=(Dispatcher&&) = delete;

    public:
        using T_Ptr = std::shared_ptr<T>;
        typedef std::map<std::string_view, T_Ptr> Ts_Map;

        virtual ~Dispatcher() = default;
        void register1(T* t) { map_[t->name()] = std::shared_ptr<T>(t); }
        void register1(T_Ptr t)
        {
            std::cerr << t->name() << " Resgistered" << std::endl;
            map_[t->name()] = t;
        }
        T_Ptr find(const std::string_view t_name) const
        {
            auto it = map_.find(t_name);

            if(it == map_.end()) {
                return std::shared_ptr<T>();
            }
            return it->second;
        }
        T_Ptr find(const std::string& t_name) const
        {
            auto it = map_.find(t_name);

            if(it == map_.end()) {
                return std::shared_ptr<T>();
            }
            return it->second;
        }
        template<typename AS> std::shared_ptr<AS> find_as() const
        {
            auto it = map_.find(AS::srv_name);

            if(it == map_.end()) {
                return std::shared_ptr<AS>();
            }
            return std::dynamic_pointer_cast<AS>(it->second);
        }
        [[nodiscard]] bool has(const std::string& t_name) const { return map_.cend() != map_.find(t_name); }
        [[nodiscard]] bool has(const std::string_view t_name) const { return map_.cend() != map_.find(t_name); }
        struct Registrator {
            Registrator(T* t, Dispatcher& d)
            {
                std::cerr << t->name() << std::endl;
                d.register1(t);
            }
            Registrator(T* t, Dispatcher* d) { d->register1(t); }
        };

    protected:
        Ts_Map map_;
    };

} // namespace trustwave

#endif /* TRUSTWAVE_COMMON_DISPATCHER_HPP_ */
