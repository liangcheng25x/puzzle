cmake_minimum_required(VERSION 2.8)

# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "\nFATAL: In-source builds are not allowed.\n"
        "You should create a separate directory for build files."
        )
endif()

# Set variables
set(PROJECT_NAME CMake_Template)
set(PROJECT_LANGUAGE C CXX)

# Environment variables
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Installation directory")
option(BUILD_TEST "Enable building test" OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build configure" FORCE)
if(DEFINED CMAKE_BUILD_TYPE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Let ycm can catch compile commands.
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Dependences paths
set(DEPS_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/hiwin_lib/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/realsense_lib/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/serial
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/hiwin_lib/deps/hrsdk_server/lib/hrsdk_ctrl
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/hiwin_lib/deps/kproto/lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/serialcontroller/lib
    )

# Library paths
set(LIB_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/hist
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/SPAData
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/SenseModel
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/PlanModel
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/ActModel
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/yaml
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/img_process
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/cf
    )

# configure
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/config)

# Utility paths
#set(UTIL_PATHS
#    ${CMAKE_CURRENT_SOURCE_DIR}/util/play
#    )

if(BUILD_TEST)
    set(UTIL_PATHS ${UTIL_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

# Set project
project(${PROJECT_NAME} ${PROJECT_LANGUAGE})

# Include cmake files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/lib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake)

