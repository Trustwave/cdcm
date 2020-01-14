cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)
project(cdcm)
# define a header-only library
add_library(protocol INTERFACE)
add_library(trustwave::protocol ALIAS protocol)
target_include_directories(protocol INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )
