cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
link_directories(${SMB_LINKS})
set(SOURCE_FILES
        cdcm_broker.cpp
        message_broker.cpp
        ../supervisor/workers_monitor.cpp
        maintenance.cpp)
add_executable(cdcm_broker ${SOURCE_FILES})
#add_subdirectory(actions)
target_link_libraries(cdcm_broker
        pthread
        zmq
        boost_system
        boost_filesystem
        boost_log
        tw-logger
        tw-common
#        tw-internal-actions
        tw-sessions-cache
        tw-zmq
        tw-singleton-runner
        )
install(TARGETS cdcm_broker
        RUNTIME DESTINATION bin
        DESTINATION ${bin_dest})
install(TARGETS cdcm_broker
        RUNTIME DESTINATION bin
        DESTINATION ${release_bin_dest})