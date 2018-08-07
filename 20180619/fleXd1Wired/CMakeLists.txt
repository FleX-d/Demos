cmake_minimum_required(VERSION 3.0)
project(fleXd1Wired VERSION "0.0.1")
set(PROJECT_DESCRIPTION "Sending temperature from 1Wired interface to mcManager")

#if(NOT CMAKE_BUILD_TYPE)
#    message(STATUS "setting default value 'Debug' for CMAKE_BUILD_TYPE")
#    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
#endif()

include(GNUInstallDirs)
include(FindPkgConfig)
find_package(PkgConfig REQUIRED)
pkg_search_module(JSON_PARSER REQUIRED jsonParser)
if (JSON_PARSER_FOUND)
    message(STATUS "JSON_PARSER libs was found: " ${JSON_PARSER_LIBRARIES})
    message(STATUS "JSON_PARSER inc was found: " ${JSON_PARSER_INCLUDE_DIRS})
endif (JSON_PARSER_FOUND)

pkg_check_modules(FLEXDIPCCONNECTOR REQUIRED fleXdIPCConnector)
pkg_check_modules(FLEXDEPOLL REQUIRED fleXdEpoll)
pkg_check_modules(FLEXDIPCCOMMON REQUIRED fleXdIPCCommon)
pkg_check_modules(FLEXDLOGGERCLIENT REQUIRED fleXdLoggerClient)

message(STATUS "FLEXDIPCCONNECTOR_INCLUDE_DIRS: "  ${FLEXDIPCCONNECTOR_INCLUDE_DIRS})
message(STATUS "FLEXDIPCCONNECTOR_LIBRARIES: "  ${FLEXDIPCCONNECTOR_LIBRARIES})
message(STATUS "FLEXDEPOLL_INCLUDE_DIRS: "  ${FLEXDEPOLL_INCLUDE_DIRS})
message(STATUS "FLEXDEPOLL_LIBRARIES: "  ${FLEXDEPOLL_LIBRARIES})
message(STATUS "FLEXDIPCCOMMON_INCLUDE_DIRS: "  ${FLEXDIPCCOMMON_INCLUDE_DIRS})
message(STATUS "FLEXDIPCCOMMON_LIBRARIES: "  ${FLEXDIPCCOMMON_LIBRARIES})
message(STATUS "FLEXDLOGGERCLIENT_LIBRARIES: "  ${FLEXDLOGGERCLIENT_LIBRARIES})

message(STATUS "***" )
message(STATUS "* " ${PROJECT_NAME})
message(STATUS "***")

set_property(
    CACHE
    CMAKE_BUILD_TYPE
    PROPERTY STRINGS
    "Debug"
    "Release"
)

set(publicHeaders
)

set(privateHeaders
    inc/IPCInterface.h
    inc/GenericClient.h
)

set(privateSources
    src/IPCInterface.cpp
    src/GenericClient.cpp
)

add_executable(${PROJECT_NAME} src/main.cpp ${privateSources})

target_include_directories(${PROJECT_NAME} PUBLIC inc/public ${JSON_PARSER_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE inc/
    ${FLEXDEPOLL_INCLUDE_DIRS}
    ${FLEXDIPCCOMMON_INCLUDE_DIRS}
    ${FLEXDLOGGERCLIENT_INCLUDE_DIRS}
    ${FLEXDIPCCONNECTOR_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)

target_link_libraries(${PROJECT_NAME}
    ${JSON_PARSER_LIBRARIES}
    ${FLEXDIPCCONNECTOR_LIBRARIES}
    ${FLEXDEPOLL_LIBRARIES}
    ${FLEXDIPCCOMMON_LIBRARIES}
    ${FLEXDLOGGERCLIENT_LIBRARIES}
)
    
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_BINARY_DIR})  


