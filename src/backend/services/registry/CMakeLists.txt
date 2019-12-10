cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

enable_cxx_compiler_flag_if_supported("-Wno-builtin-macro-redefined")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(SOURCE_FILES registry_client.cpp)
add_library(tw-registry-client STATIC ${SOURCE_FILES})
add_library(registry_common OBJECT ${SAMBA_SRC}/source4/lib/registry/tools/common.c)
add_library(s4_events OBJECT ${SAMBA_SRC}/source4/lib/events/tevent_s4.c)


target_link_libraries(tw-registry-client s4_events tevent samba-errors talloc samba-util samba-hostconfig registry-samba4 registry_common)
add_subdirectory(actions)
install(TARGETS tw-registry-client s4_events registry_common ARCHIVE DESTINATION lib
        DESTINATION ${lib_dest})
install(TARGETS tw-registry-client s4_events registry_common ARCHIVE DESTINATION lib
        DESTINATION ${release_lib_dest})