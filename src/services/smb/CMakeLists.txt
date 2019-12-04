cmake_minimum_required(VERSION 3.9.0)
project(cdcm)

link_directories(${SMB_LINKS})
include_directories(${SMB_INCLUDES})
include_directories(../../trustwave)
link_directories(/usr/local/lib/)
set (LIBPE_ROOT ${DEPS_DIR}/libpe-pev/lib/libpe)
include_directories(libfuzzy)
include_directories(libudis86)
include_directories(${LIBPE_ROOT}/include)
set(SOURCE_FILES read_file.cpp list_dir.cpp get_file_info.cpp file_exists.cpp)
add_library(tw-smb-actions OBJECT ${SOURCE_FILES})
target_link_libraries(tw-smb-actions  tw-smb-client pe_context)
install(TARGETS tw-smb-actions LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS    tw-smb-actions  LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest})