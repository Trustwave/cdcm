cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include(../../samba.cmake)

include_directories(${SMB_INCLUDES})
link_directories(${SMB_LINKS})

set(SOURCE_FILES
        registry_action.cpp
        )
add_library(tw-registry-action OBJECT ${SOURCE_FILES})
target_compile_definitions(tw-registry-action PUBLIC OBJ)
set_property(TARGET tw-registry-action PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(tw-registry-action tw-registry-client)
set(LD_ACTION_LIBS -s dl tw-registry-action)
set(ACTION_NAMES enumerate_key value_exists key_exists query_value)
#set(ACTION_NAMES enumerate_key )
foreach (X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(registry_${X} SHARED ${X}.cpp)
    target_link_libraries(registry_${X} ${LD_ACTION_LIBS})
endforeach ()

list(TRANSFORM ACTION_NAMES PREPEND "registry_" OUTPUT_VARIABLE REGISTRY_ACTIONS_LIST)
install(TARGETS ${REGISTRY_ACTIONS_LIST} DESTINATION ${lib_dest}/plugins)

install(TARGETS ${REGISTRY_ACTIONS_LIST} DESTINATION ${release_lib_dest}/plugins)
