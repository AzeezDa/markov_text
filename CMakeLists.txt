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
set(debug "${release}_dbg")

file(GLOB src "src/*.cpp")
add_compile_options(-Wall -Wpedantic -Wextra -Werror)
link_libraries(argparse)

add_executable(${release} ${src})

add_executable(${debug} ${src})
target_compile_options(${debug} PUBLIC -g)