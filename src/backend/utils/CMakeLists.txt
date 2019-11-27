cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

link_directories(/usr/local/lib/)
set (LIBPE_ROOT ${DEPS_DIR}/libpe-pev/lib/libpe)
include_directories(libfuzzy)
include_directories(libudis86)
include_directories(${LIBPE_ROOT}/include)

add_library(pe_context STATIC pe_context.cpp file_mapper.cpp )
target_link_libraries(pe_context pe boost_system boost_filesystem tw-singleton-runner)
 add_executable(runner runner.cpp file_reader.cpp )
 target_link_libraries(runner pe_context pe boost_system boost_filesystem tw-singleton-runner)

