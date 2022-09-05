cmake_minimum_required(VERSION 3.23)
set(PROJECT jomock_example)

find_package(GTest)

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

set(SOURCES
    example.cpp
)

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT}
    PUBLIC
        GTest::gtest
		GTest::gmock
        jomock
)
if (UNIX)
    target_definitions(jomock_example NON_WIN32_SUPPORT)
    if (NOT CYGWIN)
        target_link_libraries(${PROJECT} PUBLIC pthread)
    endif()
endif()

add_test(
    NAME ${PROJECT}
    COMMAND ${PROJECT}
)

