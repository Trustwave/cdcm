cmake_minimum_required(VERSION 3.9.0)
set(Boost_USE_STATIC_LIBS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
enable_testing()

include_directories(${Boost_INCLUDE_DIRS})


file(GLOB_RECURSE TEST_SRCS . *_test.cpp)
add_subdirectory(components)
add_subdirectory(functional_tests)

include(ExternalData)

#set(ExternalData_URL_TEMPLATES
#        "file://${CMAKE_CURRENT_SOURCE_DIR}/assets/%(algo)/%(hash)"
#        )
#
#ExternalData_Expand_Arguments(tests_externalData
#        pta
#        DATA{assets/pe_test.asset}
#        )
#ExternalData_Add_Target(tests_externalData)
add_executable(jsont test_json.cpp)
add_executable(tests_run test_runner.cpp ${TEST_SRCS})
#add_executable(client mdclient2.cpp)
#add_custom_command(TARGET tests_run POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:tests_run>/assets
#        COMMAND ${CMAKE_COMMAND} -E copy ${pta} $<TARGET_FILE_DIR:tests_run>/assets/
#        )
target_link_libraries(tests_run ${Boost_LIBRARIES} boost_regex tw-sessions-cache crypto pthread test_wrapper boost_filesystem tw-zmq zmq)
#add_dependencies(tests_run tests_externalData)
#target_link_libraries(client ${Boost_LIBRARIES} boost_regex tw-sessions-cache crypto pthread test_wrapper boost_filesystem tw-zmq zmq)
add_test(NAME TestName COMMAND tests_run)
if (BUILD_DEV)
    set(COVERAGE_GCOVR_EXCLUDES
            '${PROJECT_BINARY_DIR}/*'
            '${PROJECT_SOURCE_DIR}/src/tests/*'
            '${PROJECT_SOURCE_DIR}/deps/*'
            )

    SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(NAME html_coverage EXECUTABLE ctest3)
endif ()