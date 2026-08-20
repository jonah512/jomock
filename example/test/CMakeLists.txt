cmake_minimum_required(VERSION 3.8)
set(PROJECT jomock_example)

find_package(GTest CONFIG)

if (NOT GTest_FOUND)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../../googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
endif()

set(CMAKE_BUILD_TYPE Debug)
set(SOURCES
    example.cpp
)

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT}
    PUBLIC
        gtest
		gmock
)
if (UNIX)
    target_compile_definitions(jomock_example PRIVATE NON_WIN32_SUPPORT)
    if (NOT CYGWIN)
        target_link_libraries(${PROJECT} PUBLIC pthread)
    endif()
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    message("Target architecture is ARM64 (aarch64)")
    target_compile_definitions(jomock_example PRIVATE ARM64_SUPPORT)
endif()

if(MSVC)
    target_compile_options(${PROJECT} PUBLIC /MTd /Od  /ZI /RTC1 /bigobj)
    target_link_options(${PROJECT} PUBLIC /SAFESEH:NO /NODEFAULTLIB:library /subsystem:console)
endif(MSVC)

add_test(
    NAME ${PROJECT}
    COMMAND ${PROJECT}
)

