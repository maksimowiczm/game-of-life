#pragma once

#include <game_of_life/board.h>
#include <stddef.h>

#define MANAGER_ID 0

void manager_run(
    const char* output_directory,
    int workers_count,
    const int* worker_buffer_sizes,
    const Board* board,
    size_t iterations
);
