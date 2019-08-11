//===========================================================================
// Trustwave ltd. @{SRCH}
//							LoggerConfiguration.h
//
//---------------------------------------------------------------------------
// DESCRIPTION: @{HDES}
// -----------
// Represent the XML configuration
// 
//---------------------------------------------------------------------------
// CHANGES LOG: @{HREV}
// -----------
// Revision: 01.00
// By      : Avi Lachmish
// Date    : 4/12/2017 12:30:40 PM
// Comments: First Issue
//===========================================================================
#ifndef MSCPROJECT_LOGGERCONFIGURATION_H
#define MSCPROJECT_LOGGERCONFIGURATION_H
//===========================================================================
//								Include Files.
//===========================================================================
#include <string>
#include <vector>

namespace trustwave {
//===========================================================================
// @{CSEH}
//								module
//
//---------------------------------------------------------------------------
// Description    : represent source.
//===========================================================================
struct module {
    std::string name;		
    unsigned long id;		// FIXME: remove id
    unsigned long severity;
    bool enable;
};
//===========================================================================
// @{CSEH}
//								sink_conf
//
//---------------------------------------------------------------------------
// Description    : represent sink.
//===========================================================================
struct sink_conf {
    std::string path;
    std::string name;
    bool enable;
    unsigned long id;	  // FIXME: remove id
    unsigned long filter; // FIXME: rename to severity
};
//===========================================================================
//					definition of multiple sources and sinks.
//===========================================================================
using modules = std::vector<module>;
using sinks_conf = std::vector<sink_conf>;
//===========================================================================
// @{CSEH}
//							LoggerConfiguration
//
//---------------------------------------------------------------------------
// Description    : representation of the XML.
//===========================================================================
class LoggerConfiguration {
public:
    LoggerConfiguration() :
            
            project_name_("")
            {
    }
	//
	//	read configuration.
	//	-------------------
	bool load(const std::string &file_name);
	//
	//	getters.
	//	--------
	int get_configuration_version() {return version_;}
    std::string get_project_name() {return project_name_;}
    unsigned long get_sevirity_default_level() {return default_level_;}
    const sinks_conf& get_sinks() {return sinks_conf_;}
    const modules& get_modules() {return modules_;}
//
//	members.
//	--------
private:
    int version_{0};
    std::string project_name_;
    unsigned long default_level_{0};
    sinks_conf sinks_conf_ ;
    modules modules_;
};

} // namespace MSC

#endif //MSCPROJECT_LOGGERCONFIGURATION_H
