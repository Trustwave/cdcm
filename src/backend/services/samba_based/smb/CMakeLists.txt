cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include(../samba.cmake)
set(SOURCE_FILES smb_client.cpp)
add_library(tw-smb-client SHARED ${SOURCE_FILES})
target_link_libraries(tw-smb-client smbclient tw-singleton-runner)
add_subdirectory(actions)

install(TARGETS tw-smb-client LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-smb-client LIBRARY DESTINATION DESTINATION lib
        DESTINATION ${release_lib_dest})

install(FILES smb.json DESTINATION ${release_lib_dest}/plugins/)

#configure_file(smb.json ${release_lib_dest}/plugins/smb.json COPYONLY)