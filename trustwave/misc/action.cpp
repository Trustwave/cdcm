/*
 * action.cpp
 *
 *  Created on: May 15, 2019
 *      Author: root
 */
//===================================================================
//                          Include files
//===================================================================
#include <iomanip>

#include "action.hpp"
using namespace trustwave;
auto Action_Dispatcher::instance()
{
    static Action_Dispatcher dispatcher;
    return dispatcher;
}
