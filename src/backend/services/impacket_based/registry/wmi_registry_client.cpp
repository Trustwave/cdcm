//=====================================================================================================================
// Trustwave ltd. @{SRCH}
//														wmi_registry_client.cpp
//
//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:
//
//
//---------------------------------------------------------------------------------------------------------------------
// By      : Assaf Cohen
// Date    : 7/29/20
// Comments:
//=====================================================================================================================
//                          						Include files
//=====================================================================================================================

#include "wmi_registry_client.hpp"
#include <Python.h>
#include <boost/filesystem.hpp>
#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/tuple.hpp>
#include "session.hpp"
#include "credentials.hpp"
#include "../../../utils/registry_value.hpp"
#include <iostream>
#include <string>
using trustwave::wmi_registry_client;
namespace bp = boost::python;
using bpo = bp::object;
template<typename T>
inline std::vector<T> to_std_vector(const boost::python::object &iterable) {
    return std::vector<T>(bp::stl_input_iterator<T>(iterable),
                          bp::stl_input_iterator<T>());
}
bool wmi_registry_client::connect(const session& sess)
{
    try {
        Py_Initialize();
        boost::filesystem::path workingDir = boost::filesystem::absolute("./").normalize();
        PyObject* sysPath = PySys_GetObject("path");
        PyList_Insert(sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
        PySys_SetObject("path", sysPath);
        main_ = boost::python::import("__main__");
        global_ = main_.attr("__dict__");
        helper_ = boost::python::import("helper_module");
        exec_ = helper_.attr("WMI_REG_EXEC_METHOD")(sess.remote(), sess.creds().username(), sess.creds().password());
        exec_.attr("connect")();
    }
    catch (const boost::python::error_already_set &) {
        PyErr_Print();
        return false;

    } catch (...) {
        return false;
    }
    return true;
}
bool wmi_registry_client::enumerate_key(const std::string& key, enum_key& ek)
{
    try {
        auto rr = exec_.attr("EnumKey")("SOFTWARE");
        auto sn = rr.attr("sNames");
        ek.sub_keys_ = to_std_vector<sub_key>(sn);
        if(!enumerate_key_values(key,ek.registry_values_))
        {
            return false;
        }
    }
    catch (const boost::python::error_already_set &) {
        PyErr_Print();
        return false;

    } catch (...) {
        return false;
    }
    return true;
}
bool wmi_registry_client::enumerate_key_values(const std::string& key, enum_key_values& ev)
{
    try {
        auto rr = exec_.attr("EnumValues")("SOFTWARE");
        auto sn = rr.attr("sNames");
        auto ty = rr.attr("Types");
        auto vs = to_std_vector<std::string>(sn);
        auto vt = to_std_vector<uint32_t>(ty);
        for(size_t i=0;i<vs.size();++i)
        {
            registry_value rv123(vt[i],"",vs[i].c_str());
            ev.push_back(rv123);
        }

    }
    catch (const boost::python::error_already_set &) {
        PyErr_Print();
        return false;

    } catch (...) {
        return false;
    }
    return true;
}

bool wmi_registry_client::key_get_value_by_name(const std::string& key,const std::string& value, registry_value& rv)
{
    static const std::unordered_map<uint32_t,std::pair<std::string_view,std::string_view>> type_to_method=
    {
        {1, {"GetStringValue"          ,"sValue"} },
        {2, {"GetExpandedStringValue"  ,"sValue"} },
        {3, {"GetBinaryValue"          ,"uValue"} },
        {4, {"GetDWORDValue"           ,"uValue"} },
        {7, {"GetMultiStringValue"     ,"sValue"} },
        {11,{"GetQWORDValue"           ,"uValue"} }

        };
    enum_key_values ekv;
    if(enumerate_key_values(key,ekv))
    {
        auto it = std::find_if(ekv.begin(),ekv.end(),
                            [&value](const registry_value& el){return el.name_==value;});
        if(it != ekv.end())
        {
            try {
                auto rr = exec_.attr(type_to_method.at(it->type()).first.data())(key,value);
                auto sv = rr.attr(type_to_method.at(it->type()).second.data());
                rv.type(it->type());
                rv.name(it->name());
                if(it->type() == 7)
                {
//                    rv.value(to_std_vector<std::string>(sv)[0]);
                    rv.value("Fixme");
                }
                if(it->type() == 3)
                {
                    rv.value("Fixme");

                }
                rv.value(to_std_vector<std::string>(sv)[0]);
                return true;

            }
            catch (const boost::python::error_already_set &) {
                PyErr_Print();
                return false;

            } catch (...) {
                return false;
            }
        }
    }
    return false;

}
