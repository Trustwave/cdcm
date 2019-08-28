set(SOURCE_FILES test_client.cpp)

add_executable(test_client  ${SOURCE_FILES})
target_link_libraries(test_client pthread
        zmq tw-zmq)