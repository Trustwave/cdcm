//===========================================================================
// Trustwave ltd. @{SRCH}
//								close_session.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 4 Jul 2019
// Comments: 

#ifndef TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_
#define TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_
//=====================================================================================================================
//                                                  Include files
//=====================================================================================================================
#include "../../common/action.hpp"
//=====================================================================================================================
//                                                  namespaces
//=====================================================================================================================
namespace trustwave {

class Close_Session: public Action_Base
{
    static Dispatcher<Action_Base>::Registrator m_registrator;

public:
    Close_Session() :
                    Action_Base("close_session", "close_session",true)
    {
    }
    virtual int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg>, std::shared_ptr<result_msg>);
};

}
#endif /* TRUSTWAVE_SERVICES_INTERNAL_CLOSE_SESSION_HPP_ */
