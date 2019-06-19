/*
 * file_size.hpp
 *
 *  Created on: May 23, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_
#define TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_
#include "../../misc/action.hpp"

namespace trustwave {

namespace non_public{
class Get_File_Version : public Action_Base {
  static    Dispatcher<Action_Base>::Registrator m_registrator;

public:
  Get_File_Version() : Action_Base("get_file_version", "get_file_version") {}

  virtual int act(const header& header,std::shared_ptr<action_msg>,std::shared_ptr<result_msg>);
};
}
}
#endif /* TRUSTWAVE_SERVICES_LOCAL_ACTIONS_FILE_VERSION_HPP_ */
