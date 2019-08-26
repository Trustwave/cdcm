set (Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})
include_directories (..)
#Setup CMake to run tests
enable_testing()

#Prep ourselves for compiling boost

include_directories (${Boost_INCLUDE_DIRS})

#I like to keep test files in a separate source directory called test
file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *_test.cpp)

#add_subdirectory(components)
#add_subdirectory(common)
add_executable (tests_run test_runner.cpp ${TEST_SRCS} )
target_link_libraries (tests_run ${Boost_LIBRARIES} tw-sessions-cache)