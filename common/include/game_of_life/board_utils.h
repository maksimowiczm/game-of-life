#pragma once

#include <game_of_life/board.h>

enum InitType { RANDOM, PLUS, MIGRATE, HALF_PLUS, LINE };

typedef enum InitType InitType;

void init_board(const Board* board, InitType type);

void evaluate_row(
    const Cell* row,
    const Cell* prev_row,
    const Cell* next_row,
    size_t size,
    Cell* future_row
);
