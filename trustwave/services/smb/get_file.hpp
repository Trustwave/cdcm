/*
 * smb_action.hpp
 *
 *  Created on: May 15, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_
#define TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_

#include "../../misc/action.hpp"

namespace trustwave {


class SMB_Action : public Action_Base {
  static    Dispatcher<Action_Base>::Registrator m_registrator;

public:
  SMB_Action() : Action_Base("get_file", "get_file") {}

  virtual int act(const header& header,std::shared_ptr<action_msg>,std::shared_ptr<result_msg>);
};

}




#endif /* TRUSTWAVE_SERVICES_SMB_ACTIONS_SMB_ACTION_HPP_ */
