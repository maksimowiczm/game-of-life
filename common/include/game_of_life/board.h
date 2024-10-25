#pragma once
#include <stddef.h>

struct board {
  size_t width;
  size_t height;
  bool *cells;
};

typedef struct board Board;

Board *board_create(size_t width, size_t height);

void board_destroy(Board *board);

void board_set_cell(const Board *board, size_t x, size_t y, bool value);

bool board_get_cell(const Board *board, size_t x, size_t y);
