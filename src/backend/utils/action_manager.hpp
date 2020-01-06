//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//													    action_manager.hpp
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
#ifndef SRC_BACKEND_UTILS_ACTION_MANAGER_HPP
#define SRC_BACKEND_UTILS_ACTION_MANAGER_HPP
#include <boost/filesystem.hpp>

namespace trustwave {
    class Action_Base;
    class shared_library;
    template <typename T>
    class Dispatcher;
    class action_manager final{
      public:
     static  std::vector<std::shared_ptr<shared_library>> load(const boost::filesystem::path&,Dispatcher<Action_Base> &);


    };
}
#endif //SRC_BACKEND_UTILS_ACTION_MANAGER_HPP