cmake_minimum_required(VERSION 3.9.0)
project(cdcm)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
set (SAMBA_SRC /opt/samba-4.10.6)
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

message(${BOOST_LIBRARYDIR})
message(${BOOST_INCLUDEDIR})
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
include_directories(/opt/3rd/)
add_definitions(-D__STDC_WANT_LIB_EXT1__)
add_definitions(-DCMAKE_BUILD_TYPE=Debug)
link_directories(/opt/3rd/pe_pev/build)
link_directories(${SMB_LINKS})
#add_subdirectory(3rdparty)
add_subdirectory(common)
add_subdirectory(backend)
add_subdirectory(frontend)
add_subdirectory(services)
add_subdirectory(clients)
#add_subdirectory(tests)


