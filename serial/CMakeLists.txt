cmake_minimum_required(VERSION 3.20)
project(serial C)

set(CMAKE_C_STANDARD 23)

add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} common)
