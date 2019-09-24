cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

include_directories(${SMB_INCLUDES})
include_directories(../..)
link_directories(   ${SMB_LINKS})

set(SOURCE_FILES 
	registry_action.cpp   
	query_value.cpp 
	enumerate_key.cpp 
	key_exists.cpp 
	get_os.cpp
		value_exists.cpp
		)
	
add_library(tw-registry-actions OBJECT ${SOURCE_FILES})
target_compile_definitions(tw-registry-actions PUBLIC OBJ)
target_link_libraries(tw-registry-actions  tw-registry-client)
install(TARGETS tw-registry-actions LIBRARY DESTINATION lib
		DESTINATION ${lib_dest})

install(TARGETS    tw-registry-actions  LIBRARY  DESTINATION lib
		DESTINATION ${release_lib_dest})