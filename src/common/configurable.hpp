//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														configurable.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: 
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 1/1/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#ifndef SRC_COMMON_CONFIGURABLE_HPP
#define SRC_COMMON_CONFIGURABLE_HPP

#include "dispatcher.hpp"
#include "configuration.hpp"
#include "singleton_runner/settings.hpp"
namespace trustwave
{

    template <typename T>
    class configurable
    {
    protected:
        [[nodiscard]] std::shared_ptr<configuration> conf()
        {
            if(!conf_) {
                init_conf();
            }
            return conf_;
        }
        void init_conf(Dispatcher <configuration>& service_conf_reppsitory)
        {
            if(!conf_)
            {
                if(service_conf_reppsitory.has(T::srv_name))
                {
                    conf_ =std::dynamic_pointer_cast<T>( service_conf_reppsitory.find(T::srv_name));
                } else{
                    try {
                        static const auto fn = std::string(service_conf_reppsitory.find_as<cdcm_settings>()->plugins_dir_) + std::string(
                                T::srv_name) + std::string(".json");
                        const tao::json::value v = tao::json::parse_file(fn);
                        conf_ = v.as<std::shared_ptr<T>>();
                        service_conf_reppsitory.register1(conf_);
                    }
                    catch (const std::exception& e)
                    {
                        //fixme assaf handle exception
                        std::cerr<<"Failed reading "<<T::srv_name.data()<<" configuration "<<e.what()<<std::endl;
                    }
                }
            }
        }


        std::shared_ptr<T> conf_;
    };
}
#endif //SRC_COMMON_CONFIGURABLE_HPP