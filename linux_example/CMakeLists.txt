cmake_minimum_required(VERSION 3.8)
set(PROJECT jomock_example)

project(${PROJECT})

# C++ settings
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_COMPILER g++)

enable_testing()
add_subdirectory(test)

