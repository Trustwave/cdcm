//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														log_macros.hpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
//---------------------------------------------------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Assaf Cohen
// Date    : 3 Jun 2019
// Comments:

#ifndef TRUSTWAVE_COMMON_SINGLETON_RUNNER_LOG_MACROS_HPP_
#define TRUSTWAVE_COMMON_SINGLETON_RUNNER_LOG_MACROS_HPP_

#define LOGGER trustwave::authenticated_scan_server::instance().logger()
#define AU_LOG_DEBUG(...) LOGGER->log_event(   \
        trustwave::logger::debug2,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//        ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_DEBUG1(...) LOGGER->log_event(   \
        trustwave::logger::debug1,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//        ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_INFO(...) LOGGER->log_event(   \
        trustwave::logger::info,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_ERROR(...) LOGGER->log_event(   \
        trustwave::logger::error,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_WARNING(...) LOGGER->log_event(   \
        trustwave::logger::warning,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_ERRNO_DEBUG(...) LOGGER->log_event(   \
        trustwave::logger::debug,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_ERRNO_INFO(...) LOGGER->log_event(   \
        trustwave::logger::info,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

#define AU_LOG_ERRNO_ERROR(...) LOGGER->log_event(   \
        trustwave::logger::error,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)
//         ::trustwave::LoggerSource::instance()->get_source_id(),

//#define AU_LOG_DEBUG(...) printf(__VA_ARGS__)
//#define AU_LOG_ERROR(...) printf(__VA_ARGS__)
//#define AU_LOG_INFO(...) printf(__VA_ARGS__)
//#define AU_LOG_WARNING(...) printf(__VA_ARGS__)
#endif /* TRUSTWAVE_COMMON_SINGLETON_RUNNER_LOG_MACROS_HPP_ */
