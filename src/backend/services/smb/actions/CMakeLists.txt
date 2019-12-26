cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

link_directories(${SMB_LINKS})
link_directories(/usr/local/lib/)

set (LIBPE_ROOT ${DEPS_DIR}/libpe-pev/lib/libpe)

include_directories(${SMB_INCLUDES})
include_directories(libfuzzy)
include_directories(libudis86)
include_directories(${LIBPE_ROOT}/include)

set(ACTION_NAMES read_file list_dir get_file_info file_exists)
set(LD_ACTION_LIBS -s dl tw-smb-client pe_context)
foreach(X IN ITEMS ${ACTION_NAMES})
    message(STATUS "Building ${X} action")
    add_library(smb_${X} SHARED ${X}.cpp)
    set_property(TARGET smb_${X} PROPERTY POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(smb_${X} ${LD_ACTION_LIBS})
endforeach()

list(TRANSFORM ACTION_NAMES PREPEND "smb_" OUTPUT_VARIABLE SMB_ACTIONS_LIST)
install(TARGETS ${SMB_ACTIONS_LIST} LIBRARY DESTINATION lib
        DESTINATION ${lib_dest}/plugins)

install(TARGETS    ${SMB_ACTIONS_LIST}  LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest}/plugins)
