//===========================================================================
// Trustwave ltd. @{SRCH}
//								Logger.hpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
// Logger concrete implementation, In general Logger is a singleton entity per process
// (it is part of a library) which uses a configuration file in order to:
//  1.)	understand which source is enabled
//  2.)	should a source filter a message type
//  3.)	In - general which message type are allowed to be written
//  4.)	understand which sinks should we write to(currently supporting file and DebugView for windows)
//  5.)	filter messages by sink
// Loger users should usally use Initialize function in order to use the logger.
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef COMMON_LOGGER_H
#define COMMON_LOGGER_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <string>
#include <memory>

#include "../../../common/Logger/include/Enums.h"
//===========================================================================
//				Macros that should help while calling the logs.
//===========================================================================
#define WORKER_LOG       __FILE__, __FUNCTION__, __LINE__, ::trustwave::logger::worker
#define BROKER_LOG       __FILE__, __FUNCTION__, __LINE__, ::trustwave::logger::broker
#define ERR_LOG         __FILE__, __FUNCTION__, __LINE__

namespace trustwave {
namespace logger {

#define severity_levels_LIST(m)		     \
      m(severity_levels, debug, 1<<0)    \
      m(severity_levels, info,    1<<1)  \
      m(severity_levels, warning, 1<<2)  \
      m(severity_levels, error,   1<<3)
SmartEnum(severity_levels)

//enum severity_levels {
//
//    info    = 1,
//    warning = 2,
//    debug   = 4,
//    error   = 8,
//    num_of_severity_levels
//};

#define sources_LIST(m)           \
      m(sources, broker,     1<<0) \
      m(sources, worker,     1<<1) \
      m(sources, tester,    1<<2)
SmartEnum(sources)

//enum sources {
//    agent   = 1,
//    tester   = 2,
//    num_of_sources
//};

//
//	Error Auto collection.
//	----------------------
#define collect_from_system_LIST(m)					      \
      m(collect_from_system, COLLECT_FROM_NONE,     1<<0) \
      m(collect_from_system, COLLECT_FROM_SYSTEM,   1<<1) \
      m(collect_from_system, COLLECT_FROM_ERNO,     1<<2)
SmartEnum(collect_from_system)

//enum collect_from_system {
//    COLLECT_FROM_NONE   = 0,
//    COLLECT_FROM_SYSTEM = 1,
//    COLLECT_FROM_ERNO   = 2
//};

#define sinks_LIST(m)			          \
      m(sinks, file,                1<<0) \
      m(sinks, event_log,           1<<1) \
      m(sinks, output_debug_string, 1<<2)
SmartEnum(sinks)

//enum sinks {
//    file                = 1,
//    event_log           = 2,
//    output_debug_string = 4,
//    num_of_sinks
//};

constexpr unsigned long all_severity_levels = debug | info | warning | error;
constexpr unsigned long all_sources = broker | worker;
constexpr unsigned long all_collect_from_system = COLLECT_FROM_NONE | COLLECT_FROM_SYSTEM | COLLECT_FROM_ERNO;
constexpr unsigned long all_sinks = file | event_log | output_debug_string;
} //namespace logger
//===========================================================================
// @{CSEH}
//								ILogger
// 
//---------------------------------------------------------------------------
// Description    : Logger interface.
//===========================================================================
class ILogger
{
public:
    virtual ~ILogger()
    = default;

    virtual bool init(const std::string &conf_path) = 0;

    virtual void log_event(const logger::severity_levels severity, long collect, const char *file_name,
                    const char *function_name, long line_number, const trustwave::logger::sources source,
               //     const std::string_view source_id,
                    const char *format_msg, ...) = 0;
};
//===========================================================================
// @{CSEH}
//								Logger
// 
//---------------------------------------------------------------------------
// Description    : singleton that helps us retrive the logger.
// You should have a good reason to use this class usally one 
// should use the Initialize function.
//===========================================================================
class Logger final
{
public:
    ~Logger()
    = default;

    static ILogger* instance();

private:
    Logger()
    = default;
};
//===========================================================================
// @{CSEH}
//								Logger
//
//---------------------------------------------------------------------------
// Description    : singleton that helps us retrieve the logger source.
//===========================================================================
class LoggerSource
{
public:
    ~LoggerSource()
    = default;

    static LoggerSource* instance();

    void set_source(trustwave::logger::sources s,size_t id=0)
    {
        source_ = s;
        if (trustwave::logger::worker == source_) {
            source_id_ = id;
        }
    }
    trustwave::logger::sources get_source()
    {
        return source_;
    }
    const std::string get_source_id() const
    {
        static const thread_local std::string str_id = source_id_ == 0?std::string():std::to_string(source_id_);
        return str_id;
    }

private:
    LoggerSource()  
    = default;

private:
    static size_t g_id;
    trustwave::logger::sources source_{trustwave::logger::tester};
    size_t source_id_{0};
};

bool Initialize(std::unique_ptr<ILogger>& logger, const std::string &root_conf);
}
#endif //COMMON_LOGGER_H
