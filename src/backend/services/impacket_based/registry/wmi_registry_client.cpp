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
#include "registry_utils.hpp"
#include "../common/pyerror_handler.hpp"
#include "singleton_runner/authenticated_scan_server.hpp"
#include "../../../utils/session_to_client_container.hpp"
using trustwave::wmi_registry_client;
namespace bp = boost::python;
using namespace trustwave::impacket_based_common;
namespace {
    template<typename T> inline std::vector<T> to_std_vector(const boost::python::object& iterable)
    {
        return std::vector<T>(bp::stl_input_iterator<T>(iterable), bp::stl_input_iterator<T>());
    }


} // namespace
result wmi_registry_client::connect(const session& sess)
{
//    scoped_timer t("connect");
if(!connected_) {
    try {
        Py_Initialize();
        boost::filesystem::path workingDir = authenticated_scan_server::instance().settings()->plugins_dir_;
        PyObject* sysPath = PySys_GetObject("path");
        PyList_Insert(sysPath, 0, PyUnicode_FromString(workingDir.string().c_str()));
        PySys_SetObject("path", sysPath);
        main_ = boost::python::import("__main__");
//        global_ = main_.attr("__dict__");
        helper_ = boost::python::import("wmi_registry_helper");
        auto wmi_creds = sess.creds("wmi");
        exec_ = helper_.attr("WMI_REG_EXEC_METHOD")(sess.remote(), wmi_creds.username(), wmi_creds.password());
        {
            //            scoped_timer t("real connect");
            exec_.attr("connect")();
        }
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "");
    }
}
    connected_ = true;
    return std::make_tuple(true, "");
}
result wmi_registry_client::key_exists(const std::string& key, bool& exists)
{
//    scoped_timer t("key_exists");
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

    static constexpr std::string_view slashes("\\");
    auto pos = keyname.find_last_of(slashes);
    std::string parent_key;
    std::string child_key;
    if(pos == std::string::npos) {
        parent_key = std::string("");
        child_key = keyname;
    }
    else {
        parent_key = std::string(keyname.c_str(), pos);
        child_key = std::string_view(keyname.c_str() + pos + slashes.length());
    }
    exists = false;
    try {
        auto rr = exec_.attr("EnumKey")(parent_key,hive);
        try {
            if(rr.contains("Error")) {
                std::string as = bp::extract<std::string>(rr["Error"]);
                return std::make_tuple(false, as);
            }
        }
        catch(...) {
            // log?
        }
        auto vec = to_std_vector<std::string>(rr);
        exists = vec.end() != std::find_if(vec.begin(), vec.end(), [&child_key](const std::string& e) {
                     return boost::iequals(child_key, e);
                 });
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
    return std::make_tuple(true, "");
}
result
wmi_registry_client::value_exists(const std::string& key, const std::string& value, bool& exists)
{
//    scoped_timer t("value_exists");
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

    bool ex;
    auto ke_res = key_exists(key,ex);
    if(!std::get<0>(ke_res))
    {
        return ke_res;
    }
    else if(!ex)
    {
        return std::make_tuple(false, "Key Doesn't Exist");
    }
    try {
        exists = false;
        auto rr = exec_.attr("EnumValues")(keyname,hive);
        try {
            if(rr.contains("Error")) {
                std::string as = bp::extract<std::string>(rr["Error"]);
                return std::make_tuple(false, as);
            }
        }
        catch(const boost::python::error_already_set&) {
        }
        auto vec = to_std_vector<std::string>(rr["sNames"]);
        exists = vec.end() != std::find_if(vec.begin(), vec.end(), [&value](const std::string& e) {
                     return boost::iequals(value, e);
                 });
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
    return std::make_tuple(true, "");
}

result wmi_registry_client::enumerate_key(const std::string& key, enum_key& ek)
{
//    scoped_timer t("enumerate_key");
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

    bool ex;
    auto ke_res = key_exists(key,ex);
    if(!std::get<0>(ke_res))
    {
        return ke_res;
    }
    else if(!ex)
    {
        return std::make_tuple(false, "Key Doesn't Exist");
    }
    try {
        auto rr = exec_.attr("EnumKey")(keyname,hive);
        std::vector<std::string> vec;
        try {
            if(rr.contains("Error")) {
                std::string as = bp::extract<std::string>(rr["Error"]);
                return std::make_tuple(false, as);
            }
            if(!rr.is_none()) vec = to_std_vector<std::string>(rr);
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Clear();
            // no subkeys maybe values therefore will not return
        }
        catch(...) {
            return std::make_tuple(false, "");
        }
        for(auto e: vec) { ek.sub_keys_.push_back(sub_key(e)); }
        auto ekv_res = enumerate_key_values(key, ek.registry_values_);
        if(!std::get<0>(ekv_res)) {
            return ekv_res;
        }
        return std::make_tuple(true, "");
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
}
result wmi_registry_client::enumerate_key_values(const std::string& key, enum_key_values& ev)
{
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

//    scoped_timer t("enumerate_key_values");
    bool ex;
    auto ke_res = key_exists(key,ex);
    if(!std::get<0>(ke_res))
    {
        return ke_res;
    }
    else if(!ex)
    {
        return std::make_tuple(false, "Key Doesn't Exist");
    }
    try {
        auto rr = exec_.attr("EnumValues")(keyname,hive);
        try {
            if(rr.contains("Error")) {
                std::string as = bp::extract<std::string>(rr["Error"]);
                return std::make_tuple(false, as);
            }
        }
        catch(const boost::python::error_already_set&) {
            PyErr_Clear();
        }
        std::vector<std::string> vs;
        std::vector<uint32_t> vt;

        vs = to_std_vector<std::string>(rr["sNames"]);
        vt = to_std_vector<uint32_t>(rr["Types"]);

        for(size_t i = 0; i < vs.size(); ++i) {
            registry_value rv(vt[i], "", vs[i]);
            internal_key_get_value_by_name(key, vs[i], rv);
            ev.push_back(rv);
        }
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
    return std::make_tuple(true, "");
}

result
wmi_registry_client::key_get_value_by_name(const std::string& key, const std::string& value, registry_value& rv)
{
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

//    scoped_timer t("key_get_value_by_name");
    bool ex;
    auto ke_res = key_exists(key,ex);
    if(!std::get<0>(ke_res))
    {
        return ke_res;
    }
    else if(!ex)
    {
        return std::make_tuple(false, "Key Doesn't Exist");
    }
    enum_key_values ekv;
    result ekv_res;
    try {
        ekv_res = enumerate_key_values(key, ekv);
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
    if(std::get<0>(ekv_res)) {
        auto it = std::find_if(ekv.begin(), ekv.end(),
                               [&value](const registry_value& el) { return boost::iequals(el.name_, value); });
        if(it != ekv.end()) {
            rv.type(it->type());
            rv.name(it->name());
            rv.value(it->value());
            return std::make_tuple(true, "");
        }
        // value doesnt exist
        return std::make_tuple(false, "Value Doesn't Exist");
    }

    return ekv_res;
}
result
wmi_registry_client::internal_key_get_value_by_name(const std::string& key, const std::string& value,
                                                    registry_value& rv)
{
    uint32_t hive;
    std::string keyname;
    if(!trustwave::reg_hive_key(key, hive, keyname)) { return {false, "WERR_INVALID_PARAMETER"}; }

//    scoped_timer t("internal_key_get_value_by_name");
    static const std::unordered_map<uint32_t, std::string_view> type_to_method
        = {{REG_SZ, "GetStringValue"},   {REG_EXPAND_SZ, "GetExpandedStringValue"}, {REG_BINARY, "GetBinaryValue"},
           {REG_DWORD, "GetDWORDValue"}, {REG_MULTI_SZ, "GetMultiStringValue"},     {REG_QWORD, "GetQWORDValue"}};
    try {
        auto rr = exec_.attr(type_to_method.at(rv.type()).data())(keyname, value);

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
                return std::make_tuple(false, "Unknown Type");
            }
        }
        return std::make_tuple(true, "");
    }
    catch(const boost::python::error_already_set&) {
        return handle_pyerror();
    }
    catch(...) {
        AU_LOG_ERROR("non python error occurred");
        return std::make_tuple(false, "Unknown Error");
    }
}

trustwave::Dispatcher<trustwave::cdcm_client>::Registrator
    wmi_registry_client::m_registrator(new wmi_registry_client, authenticated_scan_server::instance().process_specific_repository().find_as<trustwave::sessions_to_clients>()->get_clients_dispatcher());