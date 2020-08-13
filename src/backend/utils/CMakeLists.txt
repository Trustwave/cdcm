cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(LIBPE_ROOT ${DEPS_DIR}/libpe)
link_directories(${LIBPE_ROOT})
include_directories(libfuzzy)
include_directories(libudis86)
include_directories(${LIBPE_ROOT}/include)
file(GLOB_RECURSE LIBPE_SRCS ${LIBPE_ROOT}/*.c)
add_library(static_pe STATIC ${LIBPE_SRCS})
add_library(tw-registry-utils SHARED registry_utils.cpp)

add_library(pe_context STATIC pe_context.cpp file_mapper.cpp file_reader.cpp)
set_property(TARGET pe_context PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(pe_context static_pe boost_system boost_filesystem tw-singleton-runner)

add_library(plugin_core STATIC action_manager.cpp)
target_link_libraries(plugin_core dl boost_system boost_filesystem tw-singleton-runner)

install(TARGETS tw-registry-utils  LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS  tw-registry-utils  LIBRARY DESTINATION DESTINATION lib
        DESTINATION ${release_lib_dest})





