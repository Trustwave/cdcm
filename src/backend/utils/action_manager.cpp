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
#include "../../common/dispatcher.hpp"
#include "../../common/action.hpp"
#include "shared_library.hpp"
using namespace trustwave;

std::vector<std::shared_ptr<shared_library>> action_manager::load(const boost::filesystem::path& p_dir,Dispatcher<Action_Base>& d) {
    boost::system::error_code ec;
    boost::filesystem::directory_iterator p(p_dir, ec), end_iter;
    std::vector<std::shared_ptr<shared_library>> sl_vec;
    for (; p != end_iter; ++p) {
        auto sl = std::make_shared<trustwave::shared_library >(p->path());
        auto actionl = sl->get<import_action_cb_t>("import_action");
        if(actionl)
        {
            std::cout <<p->path().filename().string() << " loaded"<<std::endl;
            auto action=actionl();
            d.register1(action);
            sl_vec.push_back(sl);
        }
        else
        {
            std::cout <<p->path().filename().string() << " failed to load"<<std::endl;
            sl->close();
        }

    }
    return std::move(sl_vec);
}