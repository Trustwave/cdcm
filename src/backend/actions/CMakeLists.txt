cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(SOURCE_FILES
#        start_session.cpp
        close_session.cpp
        )

add_library(tw-worker-internal-actions OBJECT ${SOURCE_FILES})
target_compile_definitions(tw-worker-internal-actions PUBLIC OBJ)
target_link_libraries(tw-worker-internal-actions crypto)

install(TARGETS tw-worker-internal-actions LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-worker-internal-actions LIBRARY DESTINATION lib
        DESTINATION ${release_lib_dest})