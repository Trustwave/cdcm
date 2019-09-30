cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(VERSION_MAJOR 1 CACHE STRING "major version" FORCE)
set(VERSION_MINOR 0 CACHE STRING "minor version" FORCE)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR} CACHE STRING "version" FORCE)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

include(CheckCXXCompilerFlag)
function(enable_cxx_compiler_flag_if_supported flag)
	string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
	if(flag_already_set EQUAL -1)
		check_cxx_compiler_flag("${flag}" flag_supported)
		if(flag_supported)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
		endif()
		unset(flag_supported CACHE)
	endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
#enable_cxx_compiler_flag_if_supported("-Werror")
enable_cxx_compiler_flag_if_supported("-pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
if (DEFINED ENV{DEPS_DIR})
    set (DEPS_DIR $ENV{DEPS_DIR})
    message(STATUS "Build machine")
else()
	set (DEPS_DIR /sandbox/src/deps)
    message(STATUS "Development machine")
endif()
set (SAMBA_SRC ${DEPS_DIR}/samba-4.10.6)
set (SMB_INCLUDES
   ${SAMBA_SRC}
   ${SAMBA_SRC}/include
   ${SAMBA_SRC}/param
   ${SAMBA_SRC}/ntvfs
   ${SAMBA_SRC}/lib
   ${SAMBA_SRC}/lib/crypto
   ${SAMBA_SRC}/lib/replace
   ${SAMBA_SRC}/lib/talloc
   ${SAMBA_SRC}/lib/appweb/ejs
   ${SAMBA_SRC}/lib/tdb/include
   ${SAMBA_SRC}/librpc/ndr
   ${SAMBA_SRC}/bin/default
   ${SAMBA_SRC}/bin/default/librpc/gen_ndr
   ${SAMBA_SRC}/source4 
   ${SAMBA_SRC}/source4/include
   ${SAMBA_SRC}/bin/default/include
   ${SAMBA_SRC}/bin/default/include/public
   ${SAMBA_SRC}/bin/default/source4
)
set (SMB_LINKS
	${SAMBA_SRC}/bin/shared/
	${SAMBA_SRC}/bin/shared/private/ 
	${SAMBA_SRC}/bin/default/libcli/util 
	${SAMBA_SRC}/bin/default/lib/util
	${SAMBA_SRC}/bin/default/lib/param
	${SAMBA_SRC}/bin/default/lib/tevent
	${SAMBA_SRC}/bin/default/lib/talloc
	${SAMBA_SRC}/bin/default/auth/credentials
	${SAMBA_SRC}/bin/default/source4/lib/registry
	${SAMBA_SRC}/bin/default/lib/auth/credentials
    ${SAMBA_SRC}/bin/default/source3/libsmb
	
)
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
set(Boost_USE_STATIC_LIBS ON)

if(NOT DEFINED $ENV{BOOST_INCLUDEDIR})
	set(BOOST_INCLUDEDIR /usr/include/boost169)
else()
	set(BOOST_INCLUDEDIR $ENV{BOOST_INCLUDEDIR})
endif()

if(NOT DEFINED $ENV{BOOST_LIBRARYDIR})
	set(BOOST_LIBRARYDIR /usr/lib64/boost169)
else()
	set(BOOST_LIBRARYDIR $ENV{BOOST_LIBRARYDIR})
endif()

message( STATUS ${BOOST_LIBRARYDIR})
message(STATUS ${BOOST_INCLUDEDIR})
find_package(Boost 1.69.0 COMPONENTS  )
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else (Boost_FOUND)
    message(FATAL_ERROR "no Boost for you")
endif()
message (STATUS "${BOOST_LIBRARYDIR}")
include_directories(${SMB_INCLUDES})
link_directories(${BOOST_LIBRARYDIR})
include_directories(${DEPS_DIR})
add_definitions(-DCMAKE_BUILD_TYPE=Debug)
link_directories(${DEPS_DIR}/libpe-pev/build)
link_directories(${SMB_LINKS})
configure_file(../cdcm_settings.json /var/cdcm/conf/cdcm_settings.json COPYONLY)
configure_file(../Logger/module.xml /var/cdcm/conf/Logger/module.xml COPYONLY)
set(bin_dest "bin")
set(release_bin_dest "/opt/output/bin")
set(release_lib_dest "/opt/output/libs")
set(include_dest "include/cdcm-${VERSION}")
set(lib_dest "lib/cdcm-${VERSION}")
add_subdirectory(scripts)
add_subdirectory(common)
add_subdirectory(backend)
add_subdirectory(frontend)
add_subdirectory(services)
add_subdirectory(clients)
add_subdirectory(tests)

#install(FILES Logger/module.xml cdcm_settings.json
#		DESTINATION SYSCONF
#		)


install(FILES cdcm.sh
		DESTINATION ${release_bin_dest})