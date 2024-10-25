#include <game_of_life/board.h>
#include <stdlib.h>

Board *board_create(const size_t width, const size_t height) {
  if (width == 0 || height == 0) {
    return nullptr;
  }

  const auto board = (Board *)malloc(sizeof(Board));
  const auto cells = (bool *)malloc(width * height * sizeof(bool));

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
    const bool value
) {
  const auto n = y * board->width + x;

#ifdef DEBUG
  if (n >= board->width * board->height) {
    return;
  }
#endif

  board->cells[n] = value;
}

bool board_get_cell(const Board *const board, const size_t x, const size_t y) {
  const auto n = y * board->width + x;

#ifdef DEBUG
  if (n >= board->width * board->height) {
    return false;
  }
#endif

  return board->cells[n];
}
