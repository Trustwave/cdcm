cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(SOURCE_FILES  authenticated_scan_server.cpp)
add_library(tw-singleton-runner SHARED ${SOURCE_FILES})
target_link_libraries(tw-singleton-runner tw-logger tw-sessions-cache)

install(TARGETS tw-singleton-runner LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-singleton-runner LIBRARY DESTINATION lib
        DESTINATION ${release_lib_dest})