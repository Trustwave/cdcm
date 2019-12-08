
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
set(SOURCE_FILES Logger.cpp conf/LoggerConfiguration.cpp conf/LoggerConfiguration.h)
add_library(tw-logger SHARED ${SOURCE_FILES})
target_link_libraries(tw-logger boost_log)
install(TARGETS tw-logger LIBRARY DESTINATION lib
        DESTINATION ${lib_dest})

install(TARGETS  tw-logger LIBRARY  DESTINATION lib
        DESTINATION ${release_lib_dest})