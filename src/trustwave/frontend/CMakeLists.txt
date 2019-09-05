cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

include_directories(
	..)

link_directories( ${SMB_LINKS} )

set(SOURCE_FILES 
	cdcm_broker.cpp 
	message_broker.cpp
	workers_monitor.cpp)
add_executable(cdcm_broker ${SOURCE_FILES})

target_link_libraries(cdcm_broker  
						pthread 
						zmq
						boost_system
						boost_filesystem
						boost_log
						tw-logger 
						tw-smb-actions 
						tw-common 
						tw-registry-actions  
						tw-internal-actions 
						tw-sessions-cache 
						tw-zmq 
						tw-singleton-runner
						libpe_pev.a
						)
install(TARGETS cdcm_broker RUNTIME DESTINATION bin
		DESTINATION ${bin_dest})