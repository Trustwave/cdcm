

#include_directories(${SMB_INCLUDES})
#link_directories( ${SMB_LINKS})

get_property(dirs DIRECTORY  PROPERTY LINK_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
set(SOURCE_FILES credentials.cpp session.cpp)
add_library(tw-common SHARED ${SOURCE_FILES})
target_link_libraries(tw-common  samba-credentials)

add_subdirectory(Logger)
add_subdirectory(protocol)
add_subdirectory(sessions_cache)
add_subdirectory(singleton_runner)
add_subdirectory(zmq)
