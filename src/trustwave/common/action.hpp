//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														action.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 14 May 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_ACTION_HPP_
#define TRUSTWAVE_COMMON_ACTION_HPP_

//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include <boost/core/noncopyable.hpp>  // for noncopyable
#include <string>                      // for string
#include <memory>

#include "../common/dispatcher.hpp"
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================
namespace trustwave {

struct header;
struct action_msg;
struct result_msg;
class Action_Base: public boost::noncopyable
{
public:
    Action_Base(const std::string &name, const std::string &command, bool sj = false) :
                    name_(name), command_(command), short_job_(sj)
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
    const bool short_job() const
    {
        return short_job_;
    }
    virtual int act(const header& header, std::shared_ptr<action_msg> , std::shared_ptr<result_msg> )=0;

private:
    const std::string name_;
    const std::string command_;
    const bool short_job_;
};
}
#endif /* TRUSTWAVE_COMMON_ACTION_HPP_ */
