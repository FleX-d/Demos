cmake_minimum_required(VERSION 3.0)
project(GPIOSim VERSION "0.0.1")
set(PROJECT_DESCRIPTION "FleX-d Simulator of GPIO")

message(STATUS "***" )
message(STATUS "* " ${PROJECT_NAME})
message(STATUS "***")

message(STATUS "*flexdSimIPCPublicInc: " ${flexdSimIPCPublicInc})
message(STATUS "*flexdSimIPCLib: " ${flexdSimIPCLib})

add_executable(${PROJECT_NAME} src/GPIOSim.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC inc/public)
target_include_directories(${PROJECT_NAME} PRIVATE inc/
    ${flexdSimIPCPublicInc}
    ${FLEXDEPOLL_INCLUDE_DIRS}
    ${FLEXDIPCCOMMON_INCLUDE_DIRS}
    ${FLEXDLOGGERCLIENT_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PUBLIC -std=c++14 -Wall -Wfloat-conversion)

target_link_libraries(${PROJECT_NAME}
    ${flexdSimIPCLib}
    ${FLEXDEPOLL_LIBRARIES}
    ${FLEXDIPCCOMMON_LIBRARIES}
    ${FLEXDLOGGERCLIENT_LIBRARIES}
)
    
install(TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_BINARY_DIR})
add_dependencies(${PROJECT_NAME} flexdSimIPC)    
