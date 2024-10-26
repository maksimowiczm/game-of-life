#include <game_of_life/board_utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void evaluate_row(
    const Cell* const row,
    const Cell* const prev_row,
    const Cell* const next_row,
    const size_t size,
    Cell* const future_row
) {
  for (int column = 0; column < size; column++) {
    int neighbours_count = 0;

    for (int offset = -1; offset <= 1; offset++) {
      const int new_column = column + offset;
      if (new_column < 0 || new_column >= size) {
        continue;
      }

      // check upper row
      if (prev_row && prev_row[new_column] == ALIVE) {
        neighbours_count++;
      }

      // check lower row
      if (next_row && next_row[new_column] == ALIVE) {
        neighbours_count++;
      }

      // check same row
      if (offset != 0 && row[new_column] == ALIVE) {
        neighbours_count++;
      }
    }

    // apply rules
    // https://en.wikipedia.org/wiki/Conway's_Game_of_Life#Rules
    if (row[column] == ALIVE) {
      if (neighbours_count == 2 || neighbours_count == 3) {
        future_row[column] = ALIVE;
      } else {
        future_row[column] = DEAD;
      }
    } else if (row[column] == DEAD) {
      if (neighbours_count == 3) {
        future_row[column] = ALIVE;
      } else {
        future_row[column] = DEAD;
      }
    }
  }
}

void init_board(const Board* const board, const InitType type) {
  memset(board->cells, DEAD, board_size(board) * sizeof(Cell));

  if (type == RANDOM) {
    for (size_t y = 0; y < board->height; y++) {
      for (size_t x = 0; x < board->width; x++) {
        const auto new_value = rand() / RAND_MAX < .15 ? ALIVE : DEAD;
        board_set_cell(board, x, y, new_value);
      }
    }
  } else if (type == MIGRATE) {
    if (board->width < 4 || board->height < 3) {
      fprintf(stderr, "Board is too small for MIGRATE pattern\n");
      return;
    }

    const auto half_width = board->width / 2;
    const auto half_height = board->height / 2;

    board_set_cell(board, half_width, half_height - 1, ALIVE);
    board_set_cell(board, half_width + 1, half_height, ALIVE);
    board_set_cell(board, half_width - 1, half_height + 1, ALIVE);
    board_set_cell(board, half_width, half_height + 1, ALIVE);
    board_set_cell(board, half_width + 1, half_height + 1, ALIVE);

  } else if (type == HALF_PLUS) {
    if (board->width < 3 || board->height < 2) {
      fprintf(stderr, "Board is too small for HALF_PLUS pattern\n");
      return;
    }

    const auto half_width = board->width / 2;
    const auto half_height = board->height / 2;

    board_set_cell(board, half_width, half_height - 1, ALIVE);
    board_set_cell(board, half_width - 1, half_height, ALIVE);
    board_set_cell(board, half_width, half_height, ALIVE);
    board_set_cell(board, half_width + 1, half_height, ALIVE);
  }
}
