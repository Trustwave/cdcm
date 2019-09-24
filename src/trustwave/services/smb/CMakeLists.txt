cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

link_directories(${SMB_LINKS})
include_directories(${SMB_INCLUDES})
include_directories(../..)
set(SOURCE_FILES get_file.cpp)
add_library(tw-smb-actions OBJECT ${SOURCE_FILES})
target_link_libraries(tw-smb-actions  tw-smb-client)
install(TARGETS tw-smb-actions LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS    tw-smb-actions  LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest})