cmake_minimum_required(VERSION 3.28)
project(stack VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)
FetchContent_Declare(
        libcds
        GIT_REPOSITORY https://github.com/khizmax/libcds.git
        GIT_TAG v2.3.3
)
FetchContent_MakeAvailable(libcds)
target_compile_definitions(cds PRIVATE CDS_BUILD_LIB)

file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.h")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

if (MSVC)
    add_compile_options(/utf-8 /W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -mcx16)
endif ()

add_library(core_lib STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_lib PUBLIC cds)

add_executable(start src/main.cpp)
target_link_libraries(start PRIVATE core_lib)
set_target_properties(start PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/res")

add_custom_command(TARGET start POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:cds>
        $<TARGET_FILE_DIR:start>
)