cmake_minimum_required(VERSION 3.9.0)
project(Logger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

if (WIN32)
    set(${ROOT} "X:")
elseif(APPLE)
    set(${ROOT} "/MSCProject")
else(APPLE)
    message( FATAL_ERROR "Unsupported OS. ")
endif()

set(${CMAKE_RUNTIME_LIBRARY_OUTPUT_DIRECTORY} ${ROOT}/src/Common/Bin)
set(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${ROOT}/src/Common/Bin)

#ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
SET(Boost_USE_STATIC_LIBS ON)

if (WIN32)
    # Either specify BOOST_ROOT or BOOST_INCLUDEDIR and BOOST_LIBRARYDIR.
    # I am using "MinGW Distro" in this example, but also a relative path, e. g.
    # "vendor/boost" can be specified.
    set (BOOST_ROOT "x:/3-party/boost_1_65_1")
    set (BOOST_INCLUDEDIR "x:/3-party/boost_1_65_1/")
    set (BOOST_LIBRARYDIR "x:/3-party/boost_1_65_1/lib32-msvc-14.1")
endif(WIN32)

find_package(Boost 1.65.1 COMPONENTS filesystem log REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else (Boost_FOUND)
    message(FATAL_ERROR "no Boost for you")
endif()

include_directories(${ROOT}/src/Common/include)

set(SOURCE_FILES Logger.cpp conf/LoggerConfiguration.cpp conf/LoggerConfiguration.h)
add_library(Logger ${SOURCE_FILES})
target_link_libraries(Logger ${Boost_LIBRARIES})

add_subdirectory(unittest)
