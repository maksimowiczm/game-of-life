cmake_minimum_required(VERSION 3.29)
project(common C)

set(CMAKE_C_STANDARD 23)

add_library(${PROJECT_NAME}
        src/hello.c
        src/board.c
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
