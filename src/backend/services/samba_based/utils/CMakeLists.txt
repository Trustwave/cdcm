cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include_directories(${SAMBA_SRC}/source3)
include_directories(${SAMBA_SRC}/source3/include)
link_directories(${SAMBA_SRC}/bin/default/librpc)
add_library(tw-sd-utils SHARED security_descriptor_utils.cpp)

install(TARGETS tw-sd-utils LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-sd-utils LIBRARY DESTINATION DESTINATION lib
        DESTINATION ${release_lib_dest})