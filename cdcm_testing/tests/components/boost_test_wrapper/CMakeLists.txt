set(SOURCE_FILES wrapper.cpp)
add_library(test_wrapper SHARED ${SOURCE_FILES})
target_link_libraries(test_wrapper boost_system)
