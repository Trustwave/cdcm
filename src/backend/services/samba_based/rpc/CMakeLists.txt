cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include(../samba.cmake)
include_directories(${SAMBA_SRC}/source3)
include_directories(${SAMBA_SRC}/source3/include)
link_directories(${SAMBA_SRC}/bin/default/librpc)
set(SOURCE_FILES rpc_client.cpp)
add_library(tw-rpc-client SHARED ${SOURCE_FILES})
target_link_libraries(tw-rpc-client
        tw-singleton-runner
        dcerpc-binding
        libsmb-samba4
        tevent
        samba-errors
        talloc
        ndr
        ndr-nbt
        libcli-lsa3-samba4
        libcli-netlogon3-samba4
        dcerpc-samba-samba4
        ndr-krb5pac
        ndr-samba4
        samba-util
        samba-hostconfig)

add_executable(rct rpc_client_tester.cpp)
target_link_libraries(rct tw-rpc-client dcerpc-binding
        libsmb-samba4
        tevent
        samba-errors
        talloc
        ndr-standard
        ndr
        ndr-nbt
        libcli-lsa3-samba4
        libcli-netlogon3-samba4
        dcerpc-samba-samba4
        ndr-krb5pac
        ndr-samba4
        samba-util
        samba-hostconfig)
install(TARGETS tw-rpc-client LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS tw-rpc-client LIBRARY DESTINATION DESTINATION lib
        DESTINATION ${release_lib_dest})

#install(FILES smb.json  DESTINATION ${release_lib_dest}/plugins/)

#configure_file(smb.json ${release_lib_dest}/plugins/smb.json COPYONLY)