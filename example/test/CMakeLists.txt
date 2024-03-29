cmake_minimum_required(VERSION 3.8)
set(PROJECT jomock_example)

find_package(GTest CONFIG)

if (NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG        release-1.12.1
    )
    enable_testing()
    include(GoogleTest)
    FetchContent_GetProperties(googletest)


    if(NOT googletest_POPULATED)
      FetchContent_Populate(googletest)
      add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()
endif()

set(CMAKE_BUILD_TYPE Debug)
set(SOURCES
    example.cpp
)

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT}
    PUBLIC
        GTest::gtest
		GTest::gmock
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

