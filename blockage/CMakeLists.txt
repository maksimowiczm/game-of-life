cmake_minimum_required(VERSION 3.20)
project(blockage C)

set(CMAKE_C_STANDARD 23)

add_executable(${PROJECT_NAME}
        src/main.c
        src/worker.c
)

target_link_libraries(${PROJECT_NAME} common)
target_link_libraries(${PROJECT_NAME} MPI::MPI_C OpenMP::OpenMP_C)

# If needed, you can also set the number of OpenMP threads here
set(ENV{OMP_NUM_THREADS} "4")
