cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(SOURCE_FILES Logger.cpp conf/LoggerConfiguration.cpp conf/LoggerConfiguration.h)
add_library(tw-logger SHARED ${SOURCE_FILES})
target_link_libraries(tw-logger boost_log)


