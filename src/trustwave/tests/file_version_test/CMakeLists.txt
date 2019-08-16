set(SOURCE_FILES file_version.cpp )

add_executable(file_version_t ${SOURCE_FILES})

target_link_libraries(file_version_t pe_pev crypto )
