cmake_minimum_required(VERSION 3.13)

project(simple_git)

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23)

add_executable(git ${SOURCE_FILES})
