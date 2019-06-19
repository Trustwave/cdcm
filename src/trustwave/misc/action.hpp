/*
 * action.hpp
 *
 *  Created on: May 14, 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_MISC_ACTION_HPP_
#define TRUSTWAVE_MISC_ACTION_HPP_

//===================================================================
//                          Include files
//===================================================================
#include <boost/core/noncopyable.hpp>  // for noncopyable
#include <string>                      // for string
#include "dispatcher.hpp"
namespace trustwave {

struct  header;
struct action_msg;
struct result_msg;
class Action_Base: public boost::noncopyable
{
public:
    Action_Base(const std::string &name, const std::string &command) :
                    name_(name), command_(command)
    {
    }

    virtual ~Action_Base()
    {
    }
    const std::string &name() const
    {
        return name_;
    }
    const std::string &command() const
    {
        return command_;
    }

    virtual int act(const header& header ,std::shared_ptr<action_msg>, std::shared_ptr<result_msg>) = 0;

private:
    const std::string name_;
    const std::string command_;
};
}
#endif /* TRUSTWAVE_MISC_ACTION_HPP_ */
