/*
 * query_value.hpp
 *
 *  Created on: May 20, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_
#define TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_


#include "../../misc/action.hpp"

namespace trustwave {


class Query_Value_Action : public Action_Base {
  static    Dispatcher<Action_Base>::Registrator m_registrator;

public:
  Query_Value_Action() : Action_Base("query_value", "query_value") {}

  virtual int act(const header& header,std::shared_ptr<action_msg>,std::shared_ptr<result_msg>);
};

}






#endif /* TRUSTWAVE_SERVICES_REGISTRY_ACTIONS_QUERY_VALUE_HPP_ */
