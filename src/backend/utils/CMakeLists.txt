cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

link_directories(/usr/local/lib/)
set(LIBPE_ROOT ${DEPS_DIR}/libpe)
include_directories(libfuzzy)
include_directories(libudis86)
include_directories(${LIBPE_ROOT}/include)

add_library(pe_context STATIC pe_context.cpp file_mapper.cpp file_reader.cpp)
set_property(TARGET pe_context PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(pe_context pe boost_system boost_filesystem tw-singleton-runner)

add_library(plugin_core STATIC action_manager.cpp)
target_link_libraries(plugin_core dl boost_system boost_filesystem tw-singleton-runner)
