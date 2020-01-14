cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include_directories(. /usr/local/include/openwsman)

set(SOURCE_FILES  OpenWsmanClient.cpp WsmanEPR.cpp WsmanOptions.cpp WsmanFilter.cpp)
add_library(tw-winrm-client STATIC ${SOURCE_FILES})

target_link_libraries(tw-winrm-client   wsman  wsman_client wsman_curl_client_transport xml2 curl ssl crypto pthread dl)
add_subdirectory(actions)

add_executable(my_client my_client.cpp)
target_link_libraries(my_client tw-winrm-client )

install(TARGETS my_client RUNTIME DESTINATION bin
        DESTINATION ${bin_dest})
install(TARGETS my_client
        RUNTIME DESTINATION bin
        DESTINATION ${release_bin_dest})