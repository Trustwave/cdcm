cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include(../../samba.cmake)

include_directories(${SMB_INCLUDES})
link_directories(${SMB_LINKS})

set(LD_ACTION_LIBS dl tw-lsa-client)
set(ACTION_NAMES  get_ntfs_permissions get_share_permissions)

foreach (X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(lsa_${X} SHARED ${X}.cpp)
    target_link_libraries(lsa_${X} ${LD_ACTION_LIBS})
endforeach ()

list(TRANSFORM ACTION_NAMES PREPEND "lsa_" OUTPUT_VARIABLE LSA_ACTIONS_LIST)
install(TARGETS ${LSA_ACTIONS_LIST} DESTINATION ${lib_dest}/plugins)

install(TARGETS ${LSA_ACTIONS_LIST} DESTINATION ${release_lib_dest}/plugins)
