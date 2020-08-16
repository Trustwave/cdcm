cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include_directories(/usr/include/python3.6m/)
add_library(tw-wmi-common SHARED pyerror_handler.cpp)
target_link_libraries(tw-wmi-common boost_python36 boost_system boost_filesystem python3.6m pthread dl  util m )

install(TARGETS tw-wmi-common DESTINATION ${release_lib_dest})
install(TARGETS tw-wmi-common DESTINATION ${lib_dest})

install(TARGETS tw-wmi-common ARCHIVE DESTINATION lib
        DESTINATION ${lib_dest})
install(TARGETS tw-wmi-common ARCHIVE DESTINATION lib
        DESTINATION ${release_lib_dest})


