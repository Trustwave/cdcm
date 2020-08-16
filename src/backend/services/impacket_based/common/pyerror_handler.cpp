//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														pyerror_handler.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 8/16/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================
#include "pyerror_handler.hpp"
#include <Python.h>
#include <boost/python.hpp>
namespace bp = boost::python;
using namespace trustwave::impacket_based_common;
result handle_pyerror()
{
    std::string msg;
    if(PyErr_Occurred()) {
        using namespace boost::python;
        using namespace boost;

        PyObject *exc, *val, *tb;
        object formatted_list, formatted;
        PyErr_Fetch(&exc, &val, &tb);
        handle<> hexc(exc), hval(allow_null(val)), htb(allow_null(tb));
        object traceback(import("traceback"));
        object format_exception_only(traceback.attr("format_exception_only"));
        formatted_list = format_exception_only(hexc, hval);
        formatted = str("\n").join(formatted_list);
        msg = extract<std::string>(formatted);
    }
    bp::handle_exception();
    PyErr_Clear();
    return std::make_tuple(false, msg);
}