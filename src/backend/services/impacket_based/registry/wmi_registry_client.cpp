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
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <chrono>
#include "session.hpp"
#include "base64_encode.hpp"
using trustwave::wmi_registry_client;
namespace bp = boost::python;
using bpo = bp::object;
namespace {
    template<typename T> inline std::vector<T> to_std_vector(const boost::python::object& iterable)
    {
        return std::vector<T>(bp::stl_input_iterator<T>(iterable), bp::stl_input_iterator<T>());
    }
    struct scoped_timer
    {
        explicit scoped_timer(const std::string_view name):start_(std::chrono::high_resolution_clock::now()),name_(name)
        {}
        ~scoped_timer()
        {
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_);

            std::cerr << "Time taken by "<<name_<<": "
                      << duration.count() << " microseconds" << std::endl;
        }
        decltype(std::chrono::high_resolution_clock::now()) start_;
        const std::string_view name_;
    };
} // namespace
bool wmi_registry_client::connect(const session& sess)
{
    scoped_timer t("connect");
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
        {
            scoped_timer t("real connect");
            exec_.attr("connect")();
        }
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
    return true;
}
bool wmi_registry_client::key_exists(const std::string& key, bool& exists)
{
    scoped_timer t("key_exists");
    try {
        static constexpr std::string_view slashes("\\");
        auto pos = key.find_last_of(slashes);
        std::string parent_key;
        std::string child_key;
        if(pos == std::string::npos) {
            parent_key = std::string("");
            child_key = key;
        }
        else {
            parent_key = std::string(key.c_str(), pos);
            child_key = std::string_view(key.c_str() + pos + slashes.length());
        }
        exists = false;
        auto rr = exec_.attr("EnumKey")(parent_key);
        auto sn = rr.attr("sNames");
        try {
            auto vec = to_std_vector<std::string>(sn);
            exists = vec.end() != std::find_if(vec.begin(), vec.end(), [&child_key](const std::string& e) {
                         return boost::iequals(child_key, e);
                     });
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Print();
            return true;
        }
        catch(...) {
            return false;
        }
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
    return true;
}
bool wmi_registry_client::value_exists(const std::string& key, const std::string& value, bool& exists)
{
    scoped_timer t("value_exists");
    try {
        static constexpr std::string_view slashes("\\");
        exists = false;
        auto rr = exec_.attr("EnumValues")(key);
        auto sn = rr.attr("sNames");
        try {
            auto vec = to_std_vector<std::string>(sn);
            exists = vec.end() != std::find_if(vec.begin(), vec.end(), [&value](const std::string& e) {
                         return boost::iequals(value, e);
                     });
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Print();
            return true;
        }
        catch(...) {
            return false;
        }
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
    return true;
}

bool wmi_registry_client::enumerate_key(const std::string& key, enum_key& ek)
{
    scoped_timer t("enumerate_key");
    try {
        auto rr = exec_.attr("EnumKey")(key);
        auto sn = rr.attr("sNames");
        std::vector<std::string> vec;
        try {
            vec = to_std_vector<std::string>(sn);
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Print();
            // no subkeys maybe values therefore will not return
        }
        catch(...) {
            return false;
        }
        for(auto e: vec) { ek.sub_keys_.push_back(sub_key(e)); }
        if(!enumerate_key_values(key, ek.registry_values_)) { return false; }
        return true;
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
    return false;
}
bool wmi_registry_client::enumerate_key_values(const std::string& key, enum_key_values& ev)
{
    scoped_timer t("enumerate_key_values");

    try {
        auto rr = exec_.attr("EnumValues")(key);
        auto sn = rr.attr("sNames");
        auto ty = rr.attr("Types");
        std::vector<std::string> vs;
        std::vector<uint32_t> vt;
        try {
            vs = to_std_vector<std::string>(sn);
            vt = to_std_vector<uint32_t>(ty);
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Print();
            return true;
        }
        catch(...) {
            return false;
        }
        for(size_t i = 0; i < vs.size(); ++i) {
            registry_value rv(vt[i], "", vs[i]);
            internal_key_get_value_by_name(key, vs[i], rv);
            ev.push_back(rv);
        }
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
    return true;
}

bool wmi_registry_client::key_get_value_by_name(const std::string& key, const std::string& value, registry_value& rv)
{
    scoped_timer t("key_get_value_by_name");

    enum_key_values ekv;
    if(enumerate_key_values(key, ekv)) {
        auto it = std::find_if(ekv.begin(), ekv.end(),
                               [&value](const registry_value& el) { return boost::iequals(el.name_, value); });
        if(it != ekv.end()) {
            rv.type(it->type());
            rv.name(it->name());
            return internal_key_get_value_by_name(key, value, rv);
        }
        // value doesnt exist
        return false;
    }

    return false;
}
bool wmi_registry_client::internal_key_get_value_by_name(const std::string& key, const std::string& value,
                                                         registry_value& rv)
{
    scoped_timer t("internal_key_get_value_by_name");
    static const std::unordered_map<uint32_t, std::string_view> type_to_method
        = {{REG_SZ, "GetStringValue"},   {REG_EXPAND_SZ, "GetExpandedStringValue"}, {REG_BINARY, "GetBinaryValue"},
           {REG_DWORD, "GetDWORDValue"}, {REG_MULTI_SZ, "GetMultiStringValue"},     {REG_QWORD, "GetQWORDValue"}};
    try {
        auto rr = exec_.attr(type_to_method.at(rv.type()).data())(key, value);

        switch(rv.type()) {
            case REG_BINARY: {
                auto bin_data = to_std_vector<uint8_t>(rr);
                auto c64_str = base64_encode(reinterpret_cast<char*>(bin_data.data()), bin_data.size());
                rv.value(c64_str);
            } break;
            case REG_DWORD: {
                std::stringstream stream;
                stream << "0x" << std::hex << bp::extract<uint32_t>(rr);
                rv.value(stream.str());
            } break;
            case REG_QWORD: {
                std::stringstream stream;
                stream << "0x" << std::hex << bp::extract<uint64_t>(rr);
                rv.value(stream.str());
            } break;
            case REG_SZ:
            case REG_EXPAND_SZ:
            case REG_MULTI_SZ: {
                rv.value(bp::extract<std::string>(rr));
            } break;
            default: {
                std::cerr << "in default";
                return false;
            }
        }
        return true;
    }
    catch(const boost::python::error_already_set&) {
        PyErr_Print();
        return false;
    }
    catch(...) {
        return false;
    }
}
