/*
 * get_session.hpp
 *
 *  Created on: May 20, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_
#define TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_

#include "../../misc/action.hpp"

namespace trustwave {


class Start_Session : public Action_Base {
  static    Dispatcher<Action_Base>::Registrator m_registrator;

public:
  Start_Session() : Action_Base("start_session", "start_session") {}

  virtual int act(const header& header,std::shared_ptr<action_msg>,std::shared_ptr<result_msg>);
};

}




#endif /* TRUSTWAVE_SERVICES_LOCAL_ACTIONS_GET_SESSION_HPP_ */
