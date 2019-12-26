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
#include "dispatcher.hpp"
#include <boost/core/noncopyable.hpp>  // for noncopyable
#include <boost/shared_ptr.hpp>
#include <memory>
#include <string>                      // for string
#include <utility>
#include <taocpp-json/include/tao/json/value.hpp>//fixme assaf should use fwd decl
//=====================================================================================================================
//                          						namespaces
//=====================================================================================================================

namespace trustwave {

class session;
struct action_msg;
struct result_msg;
class Action_Base: public boost::noncopyable
{
public:
    Action_Base( const std::string_view command) :
                  command_(std::move(command))
    {
    }

    virtual ~Action_Base()
    = default;

    const std::string_view command() const
    {
        return command_;
    }

    virtual int act(boost::shared_ptr <session> sess, std::shared_ptr<action_msg> , std::shared_ptr<result_msg> )=0;
    virtual std::shared_ptr<action_msg> get_message(const tao::json::value & v) const = 0;
private:
    const std::string_view command_;
};
}
using import_action_cb_t = std::shared_ptr<trustwave::Action_Base>( );

extern "C" {
    extern std::shared_ptr<trustwave::Action_Base> import_action();
}

#endif /* TRUSTWAVE_COMMON_ACTION_HPP_ */
