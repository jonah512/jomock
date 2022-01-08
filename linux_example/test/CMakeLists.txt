cmake_minimum_required(VERSION 3.8)
set(PROJECT jomock_example)

set(SOURCES
    example.cpp
)

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT}
    PUBLIC
    gtest
    gmock
    pthread
)

add_test(
    NAME ${PROJECT}
    COMMAND ${PROJECT}
)






