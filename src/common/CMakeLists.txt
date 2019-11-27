

#include_directories(${SMB_INCLUDES})
#link_directories( ${SMB_LINKS})

get_property(dirs DIRECTORY  PROPERTY LINK_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "dir='${dir}'")
endforeach()
set(SOURCE_FILES credentials.cpp session.cpp)
add_library(tw-common SHARED ${SOURCE_FILES})

add_subdirectory(Logger)
add_subdirectory(protocol)
add_subdirectory(sessions_cache)
add_subdirectory(singleton_runner)
add_subdirectory(zmq)

install(TARGETS tw-common LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS    tw-common  LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest})