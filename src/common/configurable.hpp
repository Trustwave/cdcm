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
#include "service_configuration.hpp"
namespace trustwave
{
    template <typename T>
    class configurable
    {
        [[nodiscard]] std::shared_ptr<service_configuration> conf()
        {
            if(!conf_) {
                init_conf();
            }
            return conf_;
        }
        void init_conf(Dispatcher <service_configuration>& service_conf_reppsitory)
        {
            if(!conf_)
            {

                if(service_conf_reppsitory.has(T::srv_name))
                {
                    conf_ =std::dynamic_pointer_cast<T>( service_conf_reppsitory.find(T::srv_name));
                } else{

                    try {

                        auto fn = std::string("/opt/output/libs/plugins/") + std::string(
                                T::srv_name) + std::string(".json");
                        const tao::json::value v = tao::json::parse_file(fn);
                        conf_ = v.as<std::shared_ptr<T>>();
                        service_conf_reppsitory.register1(conf_);
                    }
                    catch (const std::exception& e)
                    {
                        AU_LOG_ERROR("Failed reading %s configuration %s",T::svc_name.data(),e.what());
                    }
                }
            }
        }

    protected:
        std::shared_ptr<T> conf_;
    };
}
#endif //SRC_COMMON_CONFIGURABLE_HPP