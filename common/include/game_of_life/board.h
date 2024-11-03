#pragma once

#include <stddef.h>

enum Cell { ALIVE = 1, DEAD = 0 };

typedef enum Cell Cell;

struct board {
  size_t width;
  size_t height;
  Cell *cells;
};

typedef struct board Board;

Board *board_create(size_t width, size_t height);

void board_destroy(Board *board);

void board_set_cell(const Board *board, size_t x, size_t y, Cell value);

Cell *board_get_row(const Board *board, size_t row);

size_t board_size(const Board *board);
