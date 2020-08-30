cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(SOURCE_FILES cdcm_worker_process.cpp message_worker.cpp)
add_subdirectory(actions)
add_subdirectory(utils)
add_subdirectory(services)
add_executable(cdcm_worker ${SOURCE_FILES})
target_link_libraries(cdcm_worker
        dl
        pthread
        zmq
        boost_system
        boost_log
        tw-logger
        tw-common
        crypto
        ssl
        tw-sessions-cache
        tw-worker-internal-actions
        tw-session-to-clients
        tw-zmq
        tw-singleton-runner
        plugin_core
        )

install(TARGETS cdcm_worker RUNTIME DESTINATION bin
        DESTINATION ${bin_dest})
install(TARGETS cdcm_worker
        RUNTIME DESTINATION bin
        DESTINATION ${release_bin_dest})
