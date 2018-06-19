cmake_minimum_required(VERSION 3.0)
project(UARTSim VERSION "0.1.1")
set(PROJECT_DESCRIPTION "FleX-d Simulation of communication interface UART")

message(STATUS "***" )
message(STATUS "* " ${PROJECT_NAME})
message(STATUS "***")

message(STATUS "*flexdSimIPCPublicInc: " ${flexdSimIPCPublicInc})
message(STATUS "*flexdSimIPCLib: " ${flexdSimIPCLib})


set(publicHeaders
)

set(privateHeaders
    inc/UARTSim.h
)

set(privateSources
    src/UARTSim.cpp
    src/mainUART.cpp
)

add_executable(${PROJECT_NAME} ${privateSources} ${privateHeaders} ${publicHeaders})

target_include_directories(${PROJECT_NAME} PUBLIC inc/public)
target_include_directories(${PROJECT_NAME} PRIVATE inc/
    ${flexdSimIPCPublicInc}
    ${FLEXDEPOLL_INCLUDE_DIRS}
    ${FLEXDIPCCOMMON_INCLUDE_DIRS}
    ${FLEXDLOGGERCLIENT_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)

target_link_libraries(${PROJECT_NAME}
    ${FLEXDEPOLL_LIBRARIES}
    ${FLEXDIPCCOMMON_LIBRARIES}
    ${FLEXDLOGGERCLIENT_LIBRARIES}
    ${flexdSimIPCLib}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${publicHeaders}")
    
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_BINARY_DIR})  
    
add_dependencies(${PROJECT_NAME} flexdSimIPC)    