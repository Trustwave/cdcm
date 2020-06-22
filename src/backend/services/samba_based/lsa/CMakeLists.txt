cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(SOURCE_FILES  lsa_client.cpp)
include_directories(${SAMBA_SRC}/source3)
include_directories(${SAMBA_SRC}/source3/include)
link_directories(${SAMBA_SRC}/bin/default/librpc)

add_library(tw-lsa-client SHARED ${SOURCE_FILES})
target_link_libraries(tw-lsa-client tw-common tw-sd-utils tw-rpc-client tw-singleton-runner)
add_subdirectory(actions)
#add_executable(lct lsa_client_tester.cpp)
#target_link_libraries(lct tw-utils tw-lsa-client)


install(TARGETS tw-lsa-client LIBRARY DESTINATION lib DESTINATION ${lib_dest})

install(TARGETS tw-lsa-client LIBRARY DESTINATION DESTINATION lib
        DESTINATION ${release_lib_dest})

#install(FILES lsa.json DESTINATION ${release_lib_dest}/plugins/)

#configure_file(lsa.json ${release_lib_dest}/plugins/lsa.json COPYONLY)