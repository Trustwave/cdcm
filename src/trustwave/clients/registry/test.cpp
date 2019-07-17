//===========================================================================
// Trustwave ltd. @{SRCH}
//								test.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 1 Jul 2019
// Comments: 

#include "registry_client.hpp"

#include "registry_value.hpp"
#include "../../common/credentials.hpp"
#include "../libcli/registry/util_reg.h"
#include "../../3rdparty/json/include/tao/json.hpp"
#include "../../3rdparty/json/include/tao/json/contrib/traits.hpp"
#include <iostream>
#include <thread>
#include "../../common/session.hpp"

#ifdef __cplusplus
#include <sys/param.h>
extern "C" {
#endif
#include "lib/util/samba_util.h"
#include "libsmbclient.h"
#include "lib/util/smb_threads.h"
#include "lib/events/events.h"
#ifdef __cplusplus
}
/*
 * Define the functions which implement the pthread interface
 */
SMB_THREADS_DEF_PTHREAD_IMPLEMENTATION(tf);


/**
 * Initialize for threads using the Posix Threads (pthread)
 * implementation. This is a built-in implementation, avoiding the need to
 * implement the component functions of the thread interface. If this function
 * is used, it is not necessary to call smbc_thread_impl().
 *
 * @return {void}
 */
void
smbc_thread_posix(void)
{
        smb_thread_set_functions(&tf);
}
#endif
struct rpc_client
{
};
namespace tao {
namespace json {

template<>
struct traits<trustwave::registry_value> : binding::object<
TAO_JSON_BIND_REQUIRED ("name", &trustwave::registry_value::name_),
TAO_JSON_BIND_REQUIRED ("type", &trustwave::registry_value::type_),
TAO_JSON_BIND_REQUIRED ("value", &trustwave::registry_value::value_)>
{
};
template<>
struct traits<trustwave::sub_key> : binding::object<
TAO_JSON_BIND_REQUIRED ("name", &trustwave::sub_key::name_),
TAO_JSON_BIND_REQUIRED ("class_name", &trustwave::sub_key::class_name_),
TAO_JSON_BIND_REQUIRED ("last_modified", &trustwave::sub_key::last_modified_)>
{
};

template<>
struct traits<trustwave::enum_key> : binding::object<
TAO_JSON_BIND_REQUIRED ("sub_keyes", &trustwave::enum_key::sub_keyes_),
TAO_JSON_BIND_REQUIRED ("registry_values", &trustwave::enum_key::registry_values_)>
{
};

}
}
struct loadparm_context *lp_ctx;
namespace trustwave{
thread_local void * mem_ctx =0;
}
static void fc(int i)
{

    std::string ctxstr("top");
        ctxstr.append(std::to_string(i));
    trustwave::mem_ctx =talloc_named_const(NULL, 0,ctxstr.c_str());





    trustwave::registry_client rc;
    std::unique_ptr<trustwave::credentials> creds1;
    std::unique_ptr<trustwave::session> s;
    if (i % 2 == 0) {
        creds1 = std::make_unique<trustwave::credentials>("WORKGROUP", "%{username}", "%{password}", "SSS");
        s = std::make_unique<trustwave::session>("%{host}", *creds1);
    }
    else {
        creds1 = std::make_unique<trustwave::credentials>("WORKGROUP", "%{username}", "%{password}", "SSS");
        s = std::make_unique<trustwave::session>("%{host}", *creds1);
    }
    if (!rc.connect(*s, lp_ctx)) {
        std::cerr << "Failed to connect!!!" << std::endl;
        talloc_free(trustwave::mem_ctx);
        return;
    }
    trustwave::enum_key ek;
    rc.enumerate_key("SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion", ek);

    trustwave::enum_key ek2;
    rc.enumerate_key("SOFTWARE\\\\Microsoft\\\\Windows NT\\\\CurrentVersion", ek2);

    const tao::json::value v1 = ek;
    printf("Done %s", to_string(v1, 2).c_str());
    talloc_free(trustwave::mem_ctx);

}
int main(int argc, char **argv)
{
    talloc_disable_null_tracking();

    lp_ctx = ::loadparm_init_global(false);
    ::smbc_thread_posix();
    std::vector<std::thread> thread_pool;
for(int ii=0;ii<200;++ii){
    for (unsigned int i = 0; i < /*std::thread::hardware_concurrency() - 2*/8; ++i)        //context+broker
                    {
        thread_pool.push_back(std::move(std::thread(fc,i)));

    }
    for (unsigned int i = 0; i < thread_pool.size(); i++) {
        thread_pool.at(i).join();

    }
    thread_pool.clear();
}
//
//        for(auto k:ek.sub_keyes_)
//        {
//            std::cout<<"Name: "<<k.name_<<std::endl;
//            std::cout<<"Class name: "<<k.class_name_<<std::endl;
//            std::cout<<"Last modified: "<<k.last_modified_<<std::endl;
//        }
//        for(auto rv:ek.registry_values_)
//                {
//                    std::cout<<"Name: "<<rv.name()<<std::endl;
//                    std::cout<<"Type: "<<str_regtype(rv.type())<<std::endl;
//                    std::cout<<"Value: "<<rv.value()<<std::endl;
//                }
//
}

