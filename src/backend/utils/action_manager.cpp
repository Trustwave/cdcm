//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														action_manager.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 12/4/19
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "action_manager.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "dispatcher.hpp"
#include "action.hpp"
#include "shared_library.hpp"
using trustwave::action_manager;
using trustwave::shared_library;
using shared_lib_list = std::vector<std::shared_ptr<shared_library>>;
using directory_iterator = boost::filesystem::directory_iterator;

shared_lib_list action_manager::load(const boost::filesystem::path& p_dir, Dispatcher<Action_Base>& d)
{
    boost::system::error_code ec;
    directory_iterator p(p_dir, ec);
    directory_iterator end_iter;
    shared_lib_list sl_vec;
    std::string registered_plugins;
    for(; p != end_iter; ++p) {
        const std::regex name_lib_filter("lib\\S+\\.so");
        if(!std::regex_match(p->path().filename().generic_string(), name_lib_filter)) {
            continue;
        }
        auto sl = std::make_shared<shared_library>(p->path());
        auto actionl = sl->get<import_action_cb_t>("import_action");
        if(actionl) {
            auto action = actionl();
            d.register1(action);
            sl_vec.push_back(sl);
            AU_LOG_INFO("%s loaded.", p->path().filename().string().c_str());
            registered_plugins.append(action->name()).append(std::string(" "));
        }
        else {
            AU_LOG_INFO("%s failed to load.", p->path().filename().string().c_str());
            sl->close();
        }
    }
    registered_plugins.append(std::string("Resgistered"));
    std::cerr <<registered_plugins <<std::endl;
    return sl_vec;
}