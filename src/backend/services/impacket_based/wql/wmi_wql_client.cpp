//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_wql_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/13/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================

#include "wmi_wql_client.hpp"
#include <Python.h>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/tuple.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include "session.hpp"
#include "base64_encode.hpp"
#include "../common/pyerror_handler.hpp"

using trustwave::wmi_wql_client;
using namespace trustwave::impacket_based_common;
namespace bp = boost::python;
result wmi_wql_client::connect(const session& sess,const std::string & wmi_namespace)
{
    try {
        Py_Initialize();
        boost::filesystem::path workingDir = boost::filesystem::absolute("./").normalize();
        std::cerr << workingDir.string() << std::endl;
        PyObject* sysPath = PySys_GetObject("path");
        PyList_Insert(sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
        PySys_SetObject("path", sysPath);
        main_ = boost::python::import("__main__");
        global_ = main_.attr("__dict__");
        helper_ = boost::python::import("wmi_wql_helper");
        exec_ = helper_.attr("WMIQUERY")(sess.remote(), sess.creds().username(), sess.creds().password());
        {
//            scoped_timer t("real connect");
            exec_.attr("connect")();
        }
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        std::cerr << "A" << std::endl;
        return std::make_tuple(false, "");
    }
    catch(...) {
        return std::make_tuple(false, "");
    }
    return std::make_tuple(true, "");
}
result wmi_wql_client::query_remote_asset(const std::string & wql_query)
{

    try {
        auto rr = exec_.attr("do_query")(wql_query);
//        try {
//            if(rr.contains("Error")) {
//                std::string as = bp::extract<std::string>(rr["Error"]);
//                return std::make_tuple(false, as);
//            }
//        }
//        catch(...) {
//            // log?
//        }
        std::string query_result = bp::extract<std::string>(rr);
        return std::make_tuple(true, query_result);
    }
    catch(const boost::python::error_already_set&) {
        std::cerr << "B" << std::endl;

        return handle_pyerror();
    }
    catch(...) {
        return std::make_tuple(false, "Unknown Error");
    }
    return std::make_tuple(true, "");
}
result wmi_wql_client::close_connection()
{
    return std::make_tuple(false, "Key Doesn't Exist");
}