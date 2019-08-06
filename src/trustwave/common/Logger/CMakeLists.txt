cmake_minimum_required(VERSION 3.9.0)
project(tw-logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

#if (WIN32)
#    set(${ROOT} "X:")
#elseif(APPLE)
#    set(${ROOT} "/MSCProject")
#else(APPLE)
#    message( FATAL_ERROR "Unsupported OS. ")
#endif()

#set(${CMAKE_RUNTIME_LIBRARY_OUTPUT_DIRECTORY} ${ROOT}/src/Common/Bin)
#set(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${ROOT}/src/Common/Bin)

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
#SET(Boost_USE_STATIC_LIBS ON)
#
#find_package(Boost 1.68.0 COMPONENTS filesystem log REQUIRED)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#else (Boost_FOUND)
#    message(FATAL_ERROR "no Boost for you")
#endif()

include_directories(${ROOT}/src/Common/include)

set(SOURCE_FILES Logger.cpp conf/LoggerConfiguration.cpp conf/LoggerConfiguration.h)
add_library(tw-logger SHARED ${SOURCE_FILES})
target_link_libraries(tw-logger boost_log)


