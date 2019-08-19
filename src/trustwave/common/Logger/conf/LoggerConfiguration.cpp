//===========================================================================
// Trustwave ltd. @{SRCH}
//							LoggerConfiguration.cpp
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
//								Include Files.
//===========================================================================
#include "../../../common/Logger/conf/LoggerConfiguration.h"

#include "../include/Logger.h"
#include <map>
#include <locale>
#include <iostream>
#include <fstream>
#include <functional>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../../../common/Logger/include/Logger.h"
//===========================================================================
// @{CSEH}
//								ci_less()
//
//---------------------------------------------------------------------------
// Description: Comparator for case-insensitive comparison in STL 
// assos. containers
//===========================================================================
struct ci_less : std::binary_function<std::string, std::string, bool>
{
	//
	//	case-independent (ci) compare_less binary function.
	//	---------------------------------------------------
	struct nocase_compare : public std::binary_function<unsigned char, unsigned char, bool>
    {
        bool operator() (const unsigned char& c1, const unsigned char& c2) const {
            return tolower (c1) < tolower (c2);
        }
    };

    bool operator() (const std::string & s1, const std::string & s2) const {
        return std::lexicographical_compare(s1.begin (), s1.end (),   // source range
											s2.begin (), s2.end (),   // dest range
											nocase_compare ());	      // comparison
    }
};
//===========================================================================
// @{FUNH}
//								load()
//
//---------------------------------------------------------------------------
// Description: Read the module xml to LoggerConfiguration class
//===========================================================================
bool trustwave::LoggerConfiguration::load(const std::string &file_name) {
	//
	//	populate tree structure pt.
	//	---------------------------
	using boost::property_tree::ptree;
    ptree pt;
    boost::property_tree::xml_parser::read_xml(file_name, pt);
	//
	// Get file format version and store it in version_ 
	// variable. If wrong version fail the load!
	//	------------------------------------------------
	version_ = pt.get("Logger.version", 1);
    if (1 != version_) {
        std::cout << "unsupported verion " << version_ << std::endl;
        return false;
    }
	//
	//	Get debug project name attribute and store it in project_name_ variable.
	//	------------------------------------------------------------------------
	project_name_ = pt.get("Logger.Project.name", "No Project Name");
	//
    // Get debug level and store it in default_level_ variable. This is
    // another version of get method: if debug.level key is not
    // found, it will return default value (specified by second
    // parameter) instead of throwing. Type of the value extracted
    // is determined by type of second parameter, so we can simply
    // write get(...) instead of get<int>(...).
	//	----------------------------------------------------------
	default_level_ = pt.get("Logger.Project.level", 1);
	//
	//	traverse pt for all modules.
	//	----------------------------
	BOOST_FOREACH(ptree::value_type const&v, pt.get_child("Logger.Project.modules")) {
                    if (v.first == "module") {
                        module m;
                        m.name = v.second.get("<xmlattr>.name", "None");
                        m.id = v.second.get("<xmlattr>.id", 0);
                        m.severity = v.second.get("<xmlattr>.level", default_level_);
                        m.enable = v.second.get("<xmlattr>.enable", false);
                        modules_.push_back(m);
                    }
                }
	//
	//	string to enum.
	//	---------------
	static std::map<std::string, trustwave::logger::severity_levels, ci_less> severity_levels_to_string = {
	                                                                                                 {"debug", trustwave::logger::debug},
	                                                                                                 {"info", trustwave::logger::info},
																									 {"warn", trustwave::logger::warning},
																									 {"error", trustwave::logger::error}};
	//
	//	traverse pt for all sinks.
	//	--------------------------
	BOOST_FOREACH(ptree::value_type const&v, pt.get_child("Logger.Project.supported_sinks")) {
                    if (v.first == "sink") {
                        sink_conf s;
                        s.path = v.second.get<std::string>("path");
                        s.name = v.second.get<std::string>("name");
                        s.id = v.second.get<unsigned long>("id");
                        try {
                            s.filter = severity_levels_to_string.at(v.second.get<std::string>("filter"));
                        }
                        catch (std::out_of_range &) {
                            s.filter = trustwave::logger::info;
                        }
                        s.enable = v.second.get<bool>("enable", true);
                        sinks_conf_.push_back(s);
                    }
                }
    return true;
}
