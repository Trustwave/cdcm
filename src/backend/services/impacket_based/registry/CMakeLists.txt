cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

add_library(tw-wmi_registry-client SHARED wmi_registry_client.cpp)
target_link_libraries(tw-wmi_registry-client boost_python36 boost_system boost_filesystem python3.6m pthread dl  util m tw-common tw-singleton-runner)
include_directories(/usr/include/python3.6m/)

add_executable(wmi_registry-tester tester.cpp)
target_link_libraries(wmi_registry-tester tw-wmi_registry-client)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/helper_module.py ${CMAKE_CURRENT_BINARY_DIR}/helper_module.py COPYONLY)

#add_subdirectory(actions)
install(TARGETS tw-wmi_registry-client DESTINATION ${release_lib_dest})
install(TARGETS tw-wmi_registry-client DESTINATION ${lib_dest})

install(TARGETS tw-wmi_registry-client ARCHIVE DESTINATION lib
        DESTINATION ${lib_dest})
install(TARGETS tw-wmi_registry-client ARCHIVE DESTINATION lib
        DESTINATION ${release_lib_dest})

#configure_file(registry.json ${release_lib_dest}/plugins/registry.json COPYONLY)
