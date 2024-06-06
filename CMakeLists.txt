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

file(GLOB src "src/*.cpp")
add_compile_options(-Wall -Wpedantic -Wextra)
add_executable(${CMAKE_PROJECT_NAME} ${src})
target_link_libraries(${CMAKE_PROJECT_NAME} argparse)
