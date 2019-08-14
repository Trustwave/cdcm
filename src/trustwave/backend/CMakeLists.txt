cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
include_directories(../3rdparty ..)
link_directories( ${SMB_LINKS} )
set(SOURCE_FILES cdcm_worker_process.cpp message_worker.cpp )
add_executable(cdcm_worker ${SOURCE_FILES})
target_link_libraries(cdcm_worker pthread zmq boost_system boost_log samba-debug-samba4 smbclient-raw-samba4 krb5 
tw-logger  tw-common tw-registry-actions tw-smb-actions tw-internal-actions crypto ssl samba-credentials 
tw-sessions-cache tw-zmq tw-singleton-runner  )
