#include <game_of_life/board.h>
#include <stdio.h>
#include <stdlib.h>

Board *board_create(const size_t width, const size_t height) {
  if (width == 0 || height == 0) {
#if DEBUG
    fprintf(stderr, "Invalid board size: %zu x %zu\n", height, width);
#endif

    return nullptr;
  }

  const auto board = (Board *)malloc(sizeof(Board));
  const auto cells = (Cell *)malloc(width * height * sizeof(Cell));

  board->cells = cells;
  board->width = width;
  board->height = height;

  return board;
}

inline void board_destroy(Board *const board) {
  free(board->cells);
  free(board);
}

void board_set_cell(
    const Board *const board,
    const size_t x,
    const size_t y,
    const Cell value
) {
  const auto n = y * board->width + x;

#ifdef DEBUG
  if (n >= board_size(board)) {
    fprintf(stderr, "Invalid cell access: %zu\n", n);
    return;
  }
#endif

  board->cells[n] = value;
}

Cell *board_get_row(const Board *board, const size_t row) {
  const auto n = row * board->width;

#ifdef DEBUG
  if (n >= board_size(board)) {
    fprintf(stderr, "Invalid row access: %zu\n", n);
    return nullptr;
  }
#endif

  return board->cells + n;
}

inline size_t board_size(const Board *const board) {
  return board->width * board->height;
}