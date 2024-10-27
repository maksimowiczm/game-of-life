#pragma once

#include <stddef.h>

/**
 * @brief Represents position of a worker on the board.
 * \n
 * TOP\n
 * MIDDLE\n
 * BOTTOM\n
 */
enum WorkerType { TOP, MIDDLE, BOTTOM };

typedef enum WorkerType WorkerType;

void worker_run(
    int id,
    WorkerType worker_type,
    Board* board,
    size_t iterations,
    bool verbose
);

void worker_get_board_part(
    const Board* board,
    const int* worker_buffer_sizes,
    int worker_id,
    Cell* worker_buffer,
    int worker_buffer_size
);