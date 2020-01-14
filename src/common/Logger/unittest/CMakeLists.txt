cmake_minimum_required(VERSION 3.9.0)
project(logger_tests)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#set(${CMAKE_RUNTIME_LIBRARY_OUTPUT_DIRECTORY} ${ROOT}/OOT/lib)
set(${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${ROOT}/OOT/bin)

ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
#SET(Boost_USE_STATIC_LIBS ON)



#find_package(Boost 1.68.0 COMPONENTS filesystem log REQUIRED)
#if(Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#else (Boost_FOUND)
#    message(FATAL_ERROR "no Boost for you")
#endif()

include_directories(${ROOT}/src/Common/include)
#link_directories(${ROOT}/OOT/lib)

set(SOURCE_FILES logger_test.cpp)
add_executable(logger_test ${SOURCE_FILES})
target_link_libraries(logger_test ${Boost_LIBRARIES} Logger)
