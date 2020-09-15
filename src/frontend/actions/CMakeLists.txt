cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(SOURCE_FILES
#        start_session.cpp
#        close_session.cpp
        get_cdcm_info.cpp
        )

add_library(tw-internal-actions OBJECT ${SOURCE_FILES})
target_compile_definitions(tw-internal-actions PUBLIC OBJ)
target_link_libraries(tw-internal-actions crypto)

install(TARGETS tw-internal-actions LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-internal-actions LIBRARY DESTINATION lib
        DESTINATION ${release_lib_dest})