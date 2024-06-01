cmake_minimum_required(VERSION 3.14)
project(markov_text)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB src "src/*.cpp")

add_compile_options(-Wall -Wpedantic -Wextra)

add_executable(main ${src})