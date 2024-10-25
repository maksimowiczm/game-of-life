cmake_minimum_required(VERSION 3.29)
project(common C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(common)
add_subdirectory(async)
add_subdirectory(serial)
