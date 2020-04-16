file(GLOB RB_SRCS . *.rb *.xml *.json exe_test)
file(COPY ${RB_SRCS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
