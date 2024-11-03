cmake_minimum_required(VERSION 3.20)
project(async C)

set(CMAKE_C_STANDARD 23)

add_executable(${PROJECT_NAME}
        src/main.c
        src/worker.c
)

target_link_libraries(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME} ${MPI_C_LIBRARIES})
