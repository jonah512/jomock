cmake_minimum_required(VERSION 3.8)

project(jomock LANGUAGES CXX C)
add_library(jomock INTERFACE)
target_include_directories(jomock INTERFACE .)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples?")

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
