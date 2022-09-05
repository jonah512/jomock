cmake_minimum_required(VERSION 3.8)

project(jomocks LANGUAGES CXX C)
add_library(jomocks INTERFACE)
target_include_directories(jomocks INTERFACE .)

set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples?")

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
