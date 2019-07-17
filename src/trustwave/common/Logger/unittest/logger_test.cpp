//===========================================================================
// Trustwave ltd. @{SRCH}
//								logger_test.cpp
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
// Unit test for logger.
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef COMMON_LOGGER_TEST_H
#define COMMON_LOGGER_TEST_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <iostream>

#include "../../../common/Logger/include/Logger.h"

#ifdef _WIN32
#include <windows.h>
#endif
//===========================================================================
// @{FUNH}
//								main()
//
//---------------------------------------------------------------------------
// Description: Process entry point.
//===========================================================================
int main() {

    ::trustwave::ILogger *logger = nullptr;
    //
    //	configuration path.
    //	-------------------
#ifdef _WIN32
    const std::string root_conf("x:\\conf");
#else
    const std::string root_conf("/MSCProject/conf");
#endif
	//
	//	get the logger and process username.
	//	------------------------------------
	if (!Initialize(logger, root_conf, ::trustwave::logger::agent)) {
		std::cout << "failed to initialize the logger!!!" << std::endl;
		return -1;
	}
	//
	//	This log with different sources and different severity.
	//	-------------------------------------------------------
	logger->log_event(trustwave::logger::info, trustwave::logger::COLLECT_FROM_NONE, ERR_LOG, ::trustwave::LoggerSource::instance()->get_source(), ::trustwave::LoggerSource::instance()->get_user_name(), "Hello world");
	logger->log_event(trustwave::logger::warning, trustwave::logger::COLLECT_FROM_NONE, MANAGER_LOG, ::trustwave::LoggerSource::instance()->get_user_name(), "Hello world1 %d", 1);
	logger->log_event(trustwave::logger::error, trustwave::logger::COLLECT_FROM_NONE, TUNNEL_LOG, ::trustwave::LoggerSource::instance()->get_user_name(), "%s", "Hello world2");
	//
	//	This log also collect the errno.
	//	--------------------------------
	errno = 2;
	logger->log_event(trustwave::logger::error,trustwave::logger::COLLECT_FROM_ERNO, ERR_LOG, ::trustwave::LoggerSource::instance()->get_source(), ::trustwave::LoggerSource::instance()->get_user_name(), "Hello world3 (%s=%d)", "stam", 2);
	//
	//	This log also collect the system error (only for windows).
	//	----------------------------------------------------------
#ifdef _WIN32
	if (!CopyFile("xxx.xxx", "yyy.yyy", TRUE)) {
		logger->log_event(trustwave::logger::error, trustwave::logger::COLLECT_FROM_SYSTEM, AGENT_LOG, ::trustwave::LoggerSource::instance()->get_user_name(), "Failed to copy xxx.xxx");
	}
#endif // _WIN32

	return 0;
}
#endif //COMMON_LOGGER_TEST_H
