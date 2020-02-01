

#include "mdcliapi2.hpp"
#include <thread>
// streaming operators etc.
#include <boost/uuid/random_generator.hpp> // streaming operators etc.
#include <boost/uuid/uuid_io.hpp> // streaming operators etc.
#include <vector>

#include "protocol/protocol.hpp"
// static void enumf( std::string& msg)
//{
//    mdcli session("tcp://127.0.0.1:5555", 1);
//
//    zmsg *reply = session.send_and_recv(msg);
//    if (reply) {
//        std::cout << reply->body() << std::endl;
//        delete reply;
//    }
//
//}

static void fc(int)
{
    mdcli session("tcp://127.0.0.1:5555", 1);
    auto act_id1 = boost::uuids::random_generator()();
    std::string get_session_m =
        R"(
          { 
               "H":
               {
                   "session_id" : ")"
        + std::string("N/A") + R"("
               },
               "msgs":
                   [

                       {
                           "start_session" :
                           {
                               "id": ")"
        + boost::uuids::to_string(act_id1) + R"(",
                               "remote":"%{host}",
                               "domain":"%{domain}",
                               "username":"%{username}",
                               "password":"%{password}",
                               "workstation":"SSS"
                           }
                       }
                   ]
           })";
    zmsg* reply = session.send_and_recv(get_session_m);

    if(reply) {
        std::cout << reply->body() << std::endl;
        using namespace tao::json;
        std::string mstr(reply->body());
        const auto t1 = from_string(mstr);
        printf("msg: %s", to_string(t1, 2).c_str());

        auto a1 = t1.as<trustwave::res_msg>();
        auto act_id4 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id5 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id6 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto act_id7 = boost::uuids::to_string(boost::uuids::random_generator()());
        auto new_session_id = a1.msgs[0]->res();
        delete reply;
        reply = nullptr;

        std::string actions =
            R"(
          {      
            "H":
                {
                    "session_id" : ")"
            + new_session_id + R"("
                },
            "msgs":
                [
   
                    {
                      "winrm_enumerate"  :
                            {
                                "id": ")"
            + act_id4 + R"(",
"uri":"http://schemas.microsoft.com/wbem/wsman/1/wmi/root/cimv2/*",
"filter":"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE IpEnabled=TRUE"


                                
                            }
                    }

                ]
        })";
        printf("Request is:\n%s\n", actions.c_str());
        zmsg* reply = session.send_and_recv(actions);
        (void)reply;
    }
    reply = session.recv();
}
struct type_1 {
    int i = 8000;
    std::string s;

    std::map<std::string, std::string> m;
};
namespace tao::json {
    template<>
    struct traits<::type_1>:
        binding::object<TAO_JSON_BIND_REQUIRED("i", &type_1::i), TAO_JSON_BIND_OPTIONAL("s", &type_1::s),
                        TAO_JSON_BIND_REQUIRED("m", &type_1::m)> {
        TAO_JSON_DEFAULT_KEY("z");
    };
} // namespace tao::json

int main(int, char**)
{
    type_1 t{1, "XXX", {{"a", "1"}, {"b", "2"}, {"c", "3"}}};

    tao::json::value v = t;
    auto stm = tao::json::to_string(v);
    std::cerr << stm << std::endl;
    const auto g = v.as<type_1>();

    std::vector<std::thread> tp;
    for(unsigned int i = 0; i < 1; ++i) // context+broker
    {
        tp.push_back(std::move(std::thread(fc, 1)));
    }
    for(unsigned int i = 0; i < tp.size(); i++) {
        tp.at(i).join();
    }

    return 0;
}
