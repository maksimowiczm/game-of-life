#pragma once

#include <game_of_life/board.h>

void write_pgm(const char* file_name, const Board* board);

void parse_args(
    int argc,
    char* argv[],
    int* size,
    int* iterations,
    int* type,
    int* is_verbose
);

void print(const Board* board, int number);
