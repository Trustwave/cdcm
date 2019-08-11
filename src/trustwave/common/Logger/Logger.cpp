//===========================================================================
// Trustwave ltd. @{SRCH}
//								Logger.cpp
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
//								Include Files.
//===========================================================================
#include "../../common/Logger/include/Logger.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <pwd.h>
#include <string>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "../../common/Logger/conf/LoggerConfiguration.h"
//===========================================================================
//						Define the attribute keywords
//===========================================================================
BOOST_LOG_ATTRIBUTE_KEYWORD(file_name, "FileName", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", ::trustwave::logger::severity_levels)

namespace detail {

//===========================================================================
// @{FUNH}
//							read_error_from_errno()
//
//---------------------------------------------------------------------------
// Description: This function returns a pointer to a string that describes
// the error code passed in the argument errno
//===========================================================================
void read_error_from_errno(std::string &errno_msg) {
	unsigned int nSystemCode = errno;
	char *pErrTxt = strerror(nSystemCode);
	errno_msg += std::string("(errno=") + std::to_string(nSystemCode);
	if (pErrTxt) {
		errno_msg += std::string(" ") + pErrTxt;
	}
	errno_msg += std::string(") ");
}
//===========================================================================
//							namespace aliasing
//===========================================================================
namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace logging = boost::log;
namespace expr = boost::log::expressions;
//===========================================================================
// @{CSEH}
//								Logger
//
//---------------------------------------------------------------------------
// Description    : Logger is the class that helps us logging in the system
//===========================================================================
class Logger : public ::trustwave::ILogger {
public:
    Logger();

    ~Logger() override;

	//
	//	Logger API.
	//	-----------
	bool init(const std::string &conf_path) override;

	//
	//	Logger API.
	//	-----------
	void log_event(const ::trustwave::logger::severity_levels severity,
                           long collect,
                           const char *file_name,
                           const char *function_name,
                           long line_number,
                           const trustwave::logger::sources source,
//                           const std::string_view source_id,
                           const char* format_msg,
                           ...) override;

//
//	helper functions.
//	-----------------
private:
    bool register_sinks(::trustwave::LoggerConfiguration conf);
	void add_file_sink(const ::trustwave::sink_conf &s);
    bool internal_init(const std::string &conf_path);

private:
//    static Syslog_Client *logger_;
    boost::log::sources::severity_logger_mt<::trustwave::logger::severity_levels> lg_;
    unsigned long supported_sources_;
    unsigned long supported_default_severity_;
    unsigned long source_level_[::trustwave::logger::tester+1];
    std::mutex initialize_mutex_;
    bool initialiezed_;
};
//===========================================================================
// @{FUNH}
//									Logger()
//
//---------------------------------------------------------------------------
// Description: Constructor
//===========================================================================
Logger::Logger() :
        supported_sources_(0),
        supported_default_severity_(0),
        initialiezed_(false) {
}
//===========================================================================
// @{FUNH}
//									~Logger()
//
//---------------------------------------------------------------------------
// Description: Destructor 
//===========================================================================
Logger::~Logger() {
    logging::core::get()->flush();
    logging::core::get()->remove_all_sinks();
}

//===========================================================================
// @{FUNH}
//								add_file_sink()
//
//---------------------------------------------------------------------------
// Description: Add file sink to the logger
//===========================================================================
void Logger::add_file_sink(const ::trustwave::sink_conf &s) {
    auto fname = std::string(s.path+s.name+
                            ::trustwave::logger::sourcesArray[::trustwave::LoggerSource::instance()->get_source()] +
                                            std::string(::trustwave::LoggerSource::instance()->get_source_id())+".log.%N");
	boost::shared_ptr< sinks::synchronous_sink< sinks::text_file_backend > > sink =
		logging::add_file_log(keywords::file_name = fname.c_str(),

			keywords::rotation_size = 100 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::open_mode = (std::ios::out | std::ios::app),
			keywords::format = (
				expr::stream
				<< "["
				<< expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
					"%Y-%m-%d %H:%M:%S")
				<< "]"
				<< expr::smessage));
	sink->set_filter(severity >= s.filter);
}
//===========================================================================
// @{FUNH}
//								register_sinks()
//
//---------------------------------------------------------------------------
// Description: register all sinks that exists in configuration
//===========================================================================
bool Logger::register_sinks(::trustwave::LoggerConfiguration conf) {

    for(const auto &v : conf.get_sinks()) {
		//
		//	if the sink is disabled skip it.
		//	--------------------------------
		if (false == v.enable) continue;
		//
		//	register the sink.
		//	------------------
		switch (v.id) {
            case ::trustwave::logger::sinks::file: 
				add_file_sink(v);
                break;
            case ::trustwave::logger::sinks::event_log:
                break;
            default:
               return false;
        }
    }
    logging::add_common_attributes();
    return true;
}
//===========================================================================
// @{FUNH}
//							internal_init()
//
//---------------------------------------------------------------------------
// Description: read the configuration file and update the logger according.
//===========================================================================
bool Logger::internal_init(const std::string &conf_path) {
    //
    //	read the configuration file.
    //	----------------------------
    ::trustwave::LoggerConfiguration conf;
    conf.load(conf_path + "/Logger/module.xml");

    auto supported_severity = conf.get_sevirity_default_level();
    if (::trustwave::logger::all_severity_levels < supported_severity) {
        std::cout << "bad supported_severity - " << supported_severity << std::endl;
        return false;
    }
    //
    //	set supported severities.
    //	-------------------------
    supported_default_severity_ = supported_severity;
    //
    //	sets all sources severity level to 0.
    //	------------------------------------
    memset(&source_level_, 0, sizeof(source_level_));
    //
    //	register supported sources.
    //	---------------------------
    supported_sources_ = 0;
    for(const auto &v : conf.get_modules()) {
        if (false == v.enable) continue;
        supported_sources_ |= v.id;
        source_level_[v.id] = v.severity;
    }
    //
    //	register supported sinks.
    //	-------------------------
    if (false == register_sinks(conf)) {
        for (const auto &v : conf.get_sinks()) {
            std::cout << "failed to register a sink (it might be this sink)- " << v.id << std::endl;
        }
        return false;
    }
    //
    //  the follwing logs are written as errors so they will not be filter in any manner.
    //	---------------------------------------------------------------------------------
    BOOST_LOG_SEV(lg_, ::trustwave::logger::error) << "=====================================================================================";
    BOOST_LOG_SEV(lg_, ::trustwave::logger::error) << "                                    Log started ";
    BOOST_LOG_SEV(lg_, ::trustwave::logger::error) << "=====================================================================================";
    return true;
}
//===========================================================================
// @{FUNH}
//									init()
//
//---------------------------------------------------------------------------
// Description: read the configuration file and update the logger according.
// This function should be called once per logger.
//===========================================================================
bool Logger::init(const std::string &conf_path) {
    //
    //	promise that only one can initialize the logger.
    //	-------------------------------------------------
    std::lock_guard<std::mutex> lock(initialize_mutex_);
    if (initialiezed_) return true;

    initialiezed_ = internal_init(conf_path);
    return initialiezed_;
}
//===========================================================================
// @{FUNH}
//								log_event()
//
//---------------------------------------------------------------------------
// Description: 
//===========================================================================
void Logger::log_event(const trustwave::logger::severity_levels severity,
                       long collect,
                       const char *file_name,
                       const char *function_name,
                       long line_number,
                       const trustwave::logger::sources source,
//                       const std::string_view source_id,
                       const char *format_msg,
                       ...) {
	//
	//	unsupported source will fail to log.
	//	------------------------------------
	if ((supported_sources_ & source) == false) {
        return;
    }
	//
	//	unsupported severity_levels will fail to log.
	//	---------------------------------------------
	if ((supported_default_severity_ & severity) == false) {
        return;
    }
	//
	//	severity_levels should be enabled in the source.
	//	------------------------------------------------
	if ((source_level_[source] & severity) == false) {
        return;
    }

    //
    //	Collect more info as asked about the error.
    //	-------------------------------------------
	std::string system_msg("");

    std::string errno_msg("");
    if ((collect & ::trustwave::logger::COLLECT_FROM_ERNO) != 0) {
		read_error_from_errno(errno_msg);
    }
	//
	//	Convert file path to only the filename.
	//	---------------------------------------
	auto path_to_filename = [](std::string path) ->std::string {
        return path.substr(path.find_last_of("/\\")+1);
    };
    //
    //	User arguments list.
    //	--------------------
    va_list printf_args;
    char message[4096] = {0};
    //
    //  Init the user arguments
    //	-----------------------
    va_start(printf_args, format_msg);
    //
    //  Format the message with the givven arguments
    //	--------------------------------------------
    auto len = vsnprintf(message, sizeof message, format_msg, printf_args)+1;
    if( len < 0 )
    {
        BOOST_LOG_SEV(lg_, ::trustwave::logger::error)<<"Logger Error";
        return;
    }
    //
    //  If the result message is bigger then the
    //  buffer then cut the message in the buffer size.
    //	-----------------------------------------------
    if (static_cast<unsigned int>(len) >= sizeof message) {
        message[sizeof(message) -1] = 0;
    }
    //
    //  End using variable argument list
    //	--------------------------------
    va_end(printf_args);
    BOOST_LOG_SEV(lg_, severity)
    << " [" << std::setw(7) << std::left<< ::trustwave::logger::severity_levelsArray[severity]<<  "] "
//    << " [" << std::left<< ::trustwave::logger::sourcesArray[source] <<source_id<<"] "
	<< "[" << path_to_filename(file_name) << ":" << line_number << " " << function_name <<"()" << "] "
		<< message
		<< errno_msg
		<< system_msg;
}

} //namespace detail ended

//===========================================================================
// @{FUNH}
//								instance()
//
//---------------------------------------------------------------------------
// Description: 
//===========================================================================
//trustwave::ILogger* trustwave::Logger::instance() {
//    static detail::Logger lg;
//    return &lg;
//}
//===========================================================================
// @{FUNH}
//								instance()
//
//---------------------------------------------------------------------------
// Description:
//===========================================================================

size_t trustwave::LoggerSource::g_id =0;

trustwave::LoggerSource* trustwave::LoggerSource::instance() {
    static LoggerSource instance_;
    return &instance_;
}
//===========================================================================
// @{FUNH}
//								Initialize()
//
//---------------------------------------------------------------------------
// Description: 
//===========================================================================
bool trustwave::Initialize(std::unique_ptr<ILogger>& logger, const std::string &root_conf) {

    logger = std::make_unique<detail::Logger>();
    if (false == logger->init(root_conf)) {
        std::cout << "failed to initialize the logger!!!" << std::endl;
        return false;
    }

    return true;
}
