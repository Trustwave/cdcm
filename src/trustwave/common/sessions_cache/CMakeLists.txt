cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

set(SOURCE_FILES 
	shared_mem_converters.cpp 
	shared_mem_session.cpp 
	shared_mem_sessions_cache.cpp
	)
	
add_library(tw-sessions-cache SHARED ${SOURCE_FILES})
target_link_libraries(tw-sessions-cache tw-common)


