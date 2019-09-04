cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

include_directories(${SMB_INCLUDES})
link_directories(${SMB_LINKS})
add_library( smbclient-raw SHARED IMPORTED )
set_property( TARGET smbclient-raw PROPERTY IMPORTED_LOCATION ${SAMBA_SRC}/bin/shared/private/libsmbclient-raw-samba4.so )

add_library( smbclient SHARED IMPORTED )
set_property( TARGET smbclient PROPERTY IMPORTED_LOCATION ${SAMBA_SRC}/bin/default/source3/libsmb/libsmbclient.so )

set(SOURCE_FILES smb_downloader_client.cpp)
add_library(tw-smb-client SHARED ${SOURCE_FILES})
target_link_libraries(tw-smb-client  smbclient-raw smbclient tw-singleton-runner)

install(TARGETS tw-smb-client LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})