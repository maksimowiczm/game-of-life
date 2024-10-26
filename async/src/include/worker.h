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
    const Board* board,
    size_t iterations,
    bool verbose
);