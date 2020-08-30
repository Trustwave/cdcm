cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(ACTION_NAMES wql)

set(LD_ACTION_LIBS tw-wmi-wql-client tw-session-to-clients)

foreach(X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(wmi_${X} SHARED ${X}.cpp)
    target_link_libraries(wmi_${X} ${LD_ACTION_LIBS})
endforeach()

list(TRANSFORM ACTION_NAMES PREPEND "wmi_" OUTPUT_VARIABLE WMI_ACTIONS_LIST)
install(TARGETS ${WMI_ACTIONS_LIST} LIBRARY DESTINATION lib
        DESTINATION ${lib_dest}/plugins)

install(TARGETS ${WMI_ACTIONS_LIST}  LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest}/plugins)
