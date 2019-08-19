cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

include_directories(${SMB_INCLUDES} )

link_directories(  ${SMB_LINKS})
set(SOURCE_FILES registry_client.cpp)
add_library(tw-registry-client STATIC ${SOURCE_FILES})
#add_library(registry_common OBJECT ${SAMBA_SRC}/source4/lib/registry/tools/common.c)
#add_library(s4_events OBJECT ${SAMBA_SRC}/source4/lib/events/tevent_s4.c)
target_link_libraries(tw-registry-client s4_events tevent samba-errors talloc samba-util samba-hostconfig registry-samba4 registry_common)
