//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														get_remote_file_version.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 23 MAy 2019
// Comments:

#ifndef TRUSTWAVE_SERVICES_INTERNAL_GET_REMOTE_FILE_VERSION_HPP_
#define TRUSTWAVE_SERVICES_INTERNAL_GET_REMOTE_FILE_VERSION_HPP_
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "../../common/action.hpp"

namespace trustwave {
class Get_Remote_File_Version: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Get_Remote_File_Version() :
                    Action_Base("get_remote_file_version", "get_remote_file_version")
    {
    }

    virtual int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};
}
#endif /* TRUSTWAVE_SERVICES_INTERNAL_GET_REMOTE_FILE_VERSION_HPP_ */
