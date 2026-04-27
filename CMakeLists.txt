cmake_minimum_required(VERSION 3.21)
project(pc VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG        asio-1-30-2
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(asio)

add_library(asio_lib INTERFACE)
target_include_directories(asio_lib SYSTEM INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio_lib INTERFACE ASIO_STANDALONE _WIN32_WINNT=0x0601)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()

add_library(core_lib STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_lib PUBLIC asio_lib)

if (WIN32)
    target_link_libraries(core_lib PUBLIC ws2_32 winmm ole32)
endif ()

add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE core_lib)
set_target_properties(start PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/res")

option(BUILD_TESTING "Build the tests" ON)

if (BUILD_TESTING)
    enable_testing()
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

option(BUILD_BENCHMARKS "Build the benchmarks" ON)

if (BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            googlebenchmark
            URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googlebenchmark)

    file(GLOB_RECURSE BENCHMARK_FILES "benchmarks/*.cpp")

    if (BENCHMARK_FILES)
        add_executable(benchmarks ${BENCHMARK_FILES})
        target_link_libraries(benchmarks PRIVATE core_lib benchmark::benchmark_main)
    endif ()
endif ()
