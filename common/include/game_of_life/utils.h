#pragma once
#include <stddef.h>

enum InitType { RANDOM, PLUS, MIGRATE, HALF_PLUS };

bool* init_board(size_t size, enum InitType type);
void evaluate_row(
    const bool* row,
    const bool* prevRow,
    const bool* nextRow,
    size_t size,
    bool* futureRow
);
void write_pgm(const char* fileName, int width, int height, const bool* board);
void parse_args(
    int argc,
    char* argv[],
    int* size,
    int* iterations,
    int* type,
    int* isVerbose
);
void print(const bool* board, size_t size, int number);
