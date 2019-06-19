/*
 * log_macros.hpp
 *
 *  Created on: 3 Jun 2019
 *      Author: root
 */

#ifndef TRUSTWAVE_LOG_MACROS_HPP_
#define TRUSTWAVE_LOG_MACROS_HPP_

#define LOGGER trustwave::authenticated_scan_server::instance().logger()

#define AU_LOG_DEBUG(...) LOGGER->log_event(   \
        trustwave::logger::debug,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_INFO(...) LOGGER->log_event(   \
        trustwave::logger::info,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_ERROR(...) LOGGER->log_event(   \
        trustwave::logger::error,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_WARNING(...) LOGGER->log_event(   \
        trustwave::logger::warning,            \
        trustwave::logger::COLLECT_FROM_NONE,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_ERRNO_DEBUG(...) LOGGER->log_event(   \
        trustwave::logger::debug,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_ERRNO_INFO(...) LOGGER->log_event(   \
        trustwave::logger::info,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#define AU_LOG_ERRNO_ERROR(...) LOGGER->log_event(   \
        trustwave::logger::error,            \
        trustwave::logger::COLLECT_FROM_ERNO,                               \
        ERR_LOG,                                                            \
        ::trustwave::LoggerSource::instance()->get_source(),                \
         __VA_ARGS__)

#endif /* TRUSTWAVE_LOG_MACROS_HPP_ */
