//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														file_version.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 May 2019
// Comments:

#ifndef TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_
#define TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../../common/action.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

namespace non_public {
class Get_File_Version: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Get_File_Version() :
                    Action_Base("get_file_version", "get_file_version")
    {
    }

    int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) override;
};
}
}
#endif /* TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_ */