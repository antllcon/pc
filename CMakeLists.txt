cmake_minimum_required(VERSION 3.21)
project(pc VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ВАЖНО: Для работы boost::context (от которого зависит cobalt)
# на Windows в MSVC обязательно нужен ассемблер (MASM).
if (WIN32 AND MSVC)
    enable_language(ASM_MASM)
endif()

include(FetchContent)

set(BOOST_ENABLE_TESTING OFF CACHE BOOL "Disable Boost tests" FORCE)
set(BOOST_INCLUDE_LIBRARIES cobalt asio CACHE STRING "Build only cobalt and its dependencies" FORCE)

FetchContent_Declare(
        boost
        URL https://github.com/boostorg/boost/releases/download/boost-1.85.0/boost-1.85.0-cmake.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(boost)

# 2. Настраиваем исходники
file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()

# 3. Основная библиотека
add_library(core_lib STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Подключаем Boost (вместо старого asio_lib).
# Cobalt сам подтянет необходимые ему boost::context и boost::system.
target_link_libraries(core_lib PUBLIC Boost::cobalt Boost::asio)

if (WIN32)
    # Добавил bcrypt, так как современные версии Boost часто его требуют
    target_link_libraries(core_lib PUBLIC ws2_32 winmm ole32 bcrypt)
    # Рекомендую 0x0A00 (Windows 10/11) вместо 0x0601 (Windows 7)
    target_compile_definitions(core_lib PUBLIC _WIN32_WINNT=0x0A00)
endif ()

# 4. Исполняемый файл
add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE core_lib)
set_target_properties(start PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/res")

# 5. Тесты
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
        gtest_discover_tests(tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()
endif ()

# 6. Бенчмарки
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
        target_compile_definitions(benchmarks PRIVATE "PROJECT_ROOT_DIR=\"${CMAKE_SOURCE_DIR}\"")
    endif ()
endif ()