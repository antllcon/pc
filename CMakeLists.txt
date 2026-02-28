cmake_minimum_required(VERSION 3.18)
project(pc VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
list(REMOVE_ITEM SRC_FILES "src/main.cpp")

if (MSVC)
    set(PROJECT_WARNINGS /utf-8 /W4 /WX)
else ()
    set(PROJECT_WARNINGS -Wall -Wextra -Wpedantic -Werror)
endif ()

set(BUILD_SHARED_LIBS OFF)
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.1
)
FetchContent_MakeAvailable(SFML)

add_library(core_lib STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(core_lib PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(core_lib PUBLIC SFML::Graphics SFML::Window SFML::System)

if (WIN32)
    target_link_libraries(core_lib PUBLIC dwmapi)
endif ()

add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE core_lib)
target_compile_options(start PRIVATE ${PROJECT_WARNINGS})
set_target_properties(start PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/res")

option(BUILD_TESTING "Build the tests" ON)

if (BUILD_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_FILES "tests/*.cpp")

    if (TEST_FILES)
        add_executable(tests ${TEST_FILES})
        target_link_libraries(tests PRIVATE core_lib GTest::gmock_main)

        include(GoogleTest)
        gtest_discover_tests(tests)
    endif ()
endif ()