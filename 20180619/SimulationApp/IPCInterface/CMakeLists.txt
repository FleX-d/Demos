cmake_minimum_required(VERSION 3.0)
project(flexdSimIPC VERSION "0.1.1")
set(PROJECT_DESCRIPTION "FleX-d Simulator IPC")

message(STATUS "***" )
message(STATUS "* " ${PROJECT_NAME})
message(STATUS "***")

find_package(PkgConfig)
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

set(publicHeaders
)

set(privateHeaders
 inc/IPCInterface.h
)

set(privateSources
src/IPCInterface.cpp
)

add_library(${PROJECT_NAME} SHARED ${privateSources})

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${publicHeaders}")
    
configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

target_include_directories(${PROJECT_NAME} PUBLIC inc/public)
target_include_directories(${PROJECT_NAME} PRIVATE inc/
    ${FLEXDIPCCONNECTOR_INCLUDE_DIRS}
    ${FLEXDEPOLL_INCLUDE_DIRS}
    ${FLEXDIPCCOMMON_INCLUDE_DIRS}
    ${FLEXDLOGGERCLIENT_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)

target_link_libraries(${PROJECT_NAME}
    ${FLEXDIPCCONNECTOR_LIBRARIES}
    ${FLEXDEPOLL_LIBRARIES}
    ${FLEXDIPCCOMMON_LIBRARIES}
    ${FLEXDLOGGERCLIENT_LIBRARIES}
)
#install(TARGETS ${PROJECT_NAME}
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/libs/flexdSimIPC/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig) 
    
SET(flexdSimIPCPublicInc "${flexdSimIPC_SOURCE_DIR}/inc/" CACHE STRING "")
SET(flexdSimIPCPrivateInc "${flexdSimIPC_SOURCE_DIR}/inc" CACHE STRING "")
SET(flexdSimIPCLib "${PROJECT_NAME}" CACHE STRING "")
message(STATUS "*Export flexdSimIPCPublicInc: " ${flexdSimIPCPublicInc})
message(STATUS "*Export flexdSimIPCLib: " ${flexdSimIPCLib})
    