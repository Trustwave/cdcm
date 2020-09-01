cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(LD_ACTION_LIBS dl tw-wmi_registry-client tw-session-to-clients)
set(ACTION_NAMES wmi_authenticate wmi_enum_key wmi_enum_values wmi_key_exists wmi_value_exists wmi_query_value)
foreach (X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(wmi_registry_${X} SHARED ${X}.cpp)
    target_link_libraries(wmi_registry_${X} ${LD_ACTION_LIBS})
endforeach ()

list(TRANSFORM ACTION_NAMES PREPEND "wmi_registry_" OUTPUT_VARIABLE REGISTRY_ACTIONS_LIST)
install(TARGETS ${REGISTRY_ACTIONS_LIST} DESTINATION ${lib_dest}/plugins)

install(TARGETS ${REGISTRY_ACTIONS_LIST} DESTINATION ${release_lib_dest}/plugins)
