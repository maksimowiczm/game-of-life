cmake_minimum_required(VERSION 3.20)
project(common C)

set(CMAKE_C_STANDARD 23)

find_package(MPI REQUIRED)
find_package(OpenMP REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_subdirectory(blockage)
add_subdirectory(common)
add_subdirectory(async)
add_subdirectory(serial)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
