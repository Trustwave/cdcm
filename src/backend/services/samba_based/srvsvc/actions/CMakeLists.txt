cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include(../../samba.cmake)

include_directories(${SMB_INCLUDES})
link_directories(${SMB_LINKS})

set(LD_ACTION_LIBS dl tw-srvsvc-client)
set(ACTION_NAMES enumerate_shares)

foreach (X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(srvsvc_${X} SHARED ${X}.cpp)
    target_link_libraries(srvsvc_${X} ${LD_ACTION_LIBS})
endforeach ()

list(TRANSFORM ACTION_NAMES PREPEND "srvsvc_" OUTPUT_VARIABLE SRVSVC_ACTIONS_LIST)
install(TARGETS ${SRVSVC_ACTIONS_LIST} DESTINATION ${lib_dest}/plugins)

install(TARGETS ${SRCSVC_ACTIONS_LIST} DESTINATION ${release_lib_dest}/plugins)
