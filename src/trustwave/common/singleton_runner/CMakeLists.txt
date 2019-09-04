cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include_directories(../..)
link_directories(${SMB_LINKS})
set(SOURCE_FILES authenticated_scan_server.cpp settings.cpp)
add_library(tw-singleton-runner SHARED ${SOURCE_FILES})
target_link_libraries(tw-singleton-runner tw-zmq tw-logger tw-sessions-cache)

install(TARGETS tw-singleton-runner LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

