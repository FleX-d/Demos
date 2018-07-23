cmake_minimum_required(VERSION 3.0)
project(DockerApp VERSION "0.1")
set(PROJECT_DESCRIPTION "DockerApp for FleXd Demo")

message(STATUS "***" )
message(STATUS "* " ${PROJECT_NAME})
message(STATUS "***")

include(FindPkgConfig)
find_package(PkgConfig REQUIRED)
pkg_search_module(JSON_PARSER REQUIRED jsonParser)
if (JSON_PARSER_FOUND)
    message(STATUS "JSON_PARSER libs was found: " ${JSON_PARSER_LIBRARIES})
    message(STATUS "JSON_PARSER inc was found: " ${JSON_PARSER_INCLUDE_DIRS})
endif (JSON_PARSER_FOUND)

pkg_search_module(FLEXD_LOGGER REQUIRED fleXdLoggerClient)
if (FLEXD_LOGGER_FOUND)
    message(STATUS "FLEXD_LOGGER libs was found: " ${FLEXD_LOGGER_LIBRARIES})
    message(STATUS "FLEXD_LOGGER inc was found: " ${FLEXD_LOGGER_INCLUDE_DIRS})
endif (FLEXD_LOGGER_FOUND)

pkg_search_module(FLEXD_EPOLL REQUIRED fleXdEpoll)
if (FLEXD_EPOLL_FOUND)
    message(STATUS "FLEXD_EPOLL libs was found: " ${FLEXD_EPOLL_LIBRARIES})
    message(STATUS "FLEXD_EPOLL inc was found: " ${FLEXD_EPOLL_INCLUDE_DIRS})
endif (FLEXD_EPOLL_FOUND)

pkg_search_module(FLEXD_CONECTOR REQUIRED fleXdIPCConnector)
if (FLEXD_CONECTOR_FOUND)
    message(STATUS "FLEXD_CONECTOR libs was found: " ${FLEXD_CONECTOR_LIBRARIES})
    message(STATUS "FLEXD_CONECTOR inc was found: " ${FLEXD_CONECTOR_INCLUDE_DIRS})
endif (FLEXD_CONECTOR_FOUND)

pkg_search_module(FLEXD_IPCCOMMON REQUIRED fleXdIPCCommon)
if (FLEXD_IPCCOMMON_FOUND)
    message(STATUS "FLEXD_IPCCOMMON libs was found: " ${FLEXD_IPCCOMMON_LIBRARIES})
    message(STATUS "FLEXD_IPCCOMMON inc was found: " ${FLEXD_IPCCOMMON_INCLUDE_DIRS})
endif (FLEXD_IPCCOMMON_FOUND)

pkg_search_module(FLEXD_INIPARSER REQUIRED iniparser)
if (FLEXD_INIPARSER_FOUND)
    message(STATUS "FLEXD_INIPARSER libs was found: " ${FLEXD_INIPARSER_LIBRARIES})
    message(STATUS "FLEXD_INIPARSER inc was found: " ${FLEXD_INIPARSER_INCLUDE_DIRS})
endif (FLEXD_INIPARSER_FOUND)

SET(privateHeaders
    inc/IPCClient.h
    inc/IPCInterface.h
    inc/GenericClient.h
)

SET(privateSources
    src/IPCClient.cpp
    src/IPCInterface.cpp
    src/GenericClient.cpp
    src/main.cpp
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/)

add_executable(${PROJECT_NAME} ${privateSources} ${privateHeaders})
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion ${JSON_PARSER_CFLAGS_OTHER})
target_include_directories(${PROJECT_NAME} PUBLIC )
target_include_directories(${PROJECT_NAME} PRIVATE inc
    ${JSON_PARSER_INCLUDE_DIRS}
    ${FLEXD_LOGGER_INCLUDE_DIRS}
    ${FLEXD_EPOLL_INCLUDE_DIRS}
    ${FLEXD_CONECTOR_INCLUDE_DIRS}
    ${FLEXD_IPCCOMMON_INCLUDE_DIRS}
    ${FLEXD_INIPARSER_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
    ${JSON_PARSER_LIBRARIES}
    ${FLEXD_LOGGER_LIBRARIES}
    ${FLEXD_EPOLL_LIBRARIES}
    ${FLEXD_CONECTOR_LIBRARIES}
    ${FLEXD_IPCCOMMON_LIBRARIES}
    ${FLEXD_INIPARSER_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
