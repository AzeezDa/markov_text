cmake_minimum_required(VERSION 3.12)
project(markov_text)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

set(release ${CMAKE_PROJECT_NAME})

file(GLOB src "src/*.cpp")
link_libraries(argparse)

add_executable(${release} ${src})


message("${CMAKE_CXX_COMPILER_ID}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang") 
    set(debug "${release}_dbg")
    add_executable(${debug} ${src})
    add_compile_options(-Wall -Wpedantic -Wextra -Werror)
    target_compile_options(${release} PUBLIC -O3)
    target_compile_options(${debug} PUBLIC -g)
endif()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" REQUIRED)
if (CLANG_TIDY_EXE)
    message("Using clang-tidy")
    set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "--format-style=file" "-checks=-*,readability-*,modernize-*,bugprone-*,performance-*,misc-*")
    set_target_properties(${debug} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()