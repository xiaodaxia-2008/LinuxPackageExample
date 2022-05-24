# Copyright © 2022 Zen Shawn. All rights reserved. Created at 15:47:23, May 19,
# 2022

cmake_minimum_required(VERSION 3.13)

option(BUILD_SHARED_LIBS "build shared libs" ON)
set(CMAKE_INSTALL_PREFIX
    "$ENV{HOME}/Programs/Temp"
    CACHE PATH "install prefix")
set(CMAKE_BUILD_TYPE
    "RelWithDebInfo"
    CACHE STRING "build type, Release/Debug/MinSizeRel/RelWithDebInfo")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
cmake_policy(SET CMP0087 NEW)

project(Temp)
set(CMAKE_INSTALL_PREFIX
    $ENV{HOME}/Rvbust/Install/${PROJECT_NAME}
    CACHE PATH "Install prefix" FORCE)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache")
  set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

set(RPS_DIR $ENV{HOME}/Rvbust/Install/RVBUST/RPS/Lib/CMake)
find_package(
  RPS
  COMPONENTS Controller
  REQUIRED)
find_package(pybind11 REQUIRED)

add_library(ku ku.cpp)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE ku)

pybind11_add_module(pyku pyku.cpp)
target_link_libraries(pyku PUBLIC ku)