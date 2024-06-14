cmake_minimum_required(VERSION 3.11)
project(markov_text)

set(CMAKE_CXX_STANDARD 17)
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