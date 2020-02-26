cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(SOURCE_FILES
        supervisor.cpp
        workers_monitor.cpp
        )
add_executable(cdcm_supervisor ${SOURCE_FILES})

target_link_libraries(cdcm_supervisor
        pthread
        boost_system
        boost_filesystem
        boost_log
        tw-logger
        zmq
        tw-zmq
        tw-singleton-runner
        )
install(TARGETS cdcm_supervisor
        RUNTIME DESTINATION bin
        DESTINATION ${bin_dest})
install(TARGETS cdcm_supervisor
        RUNTIME DESTINATION bin
        DESTINATION ${release_bin_dest})