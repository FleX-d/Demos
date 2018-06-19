cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "setting default value 'Debug' for CMAKE_BUILD_TYPE")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

option(
    BUILD_DEMO_20180619
    "DEMO 20180619"
    ON
    )
#option(
#    BUILD_DEMO_XXXXXXXX
#    "DEMO XXXXXXXX"
#    OFF
#    )
    
message(STATUS "option BUILD_DEMO_20180619=" ${BUILD_DEMO_20180619})
#message(STATUS "option BUILD_DEMO_XXXXXXXX=" ${BUILD_DEMO_XXXXXXXX})

if(BUILD_DEMO_20180619)
  add_subdirectory(20180619)
endif (BUILD_DEMO_20180619)

#if(BUILD_DEMO_XXXXXXXX)
#  add_subdirectory(BUILD_DEMO_XXXXXXXX)
#endif (BUILD_DEMO_XXXXXXXX)