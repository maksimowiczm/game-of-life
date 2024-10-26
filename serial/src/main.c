#include <game_of_life/board_utils.h>
#include <game_of_life/utils.h>
#include <stdlib.h>
#include <time.h>

// ffmpeg -framerate 3 -i life%d.pgm -vf "scale=iw*10:ih*10" -c:v libx264
// -pix_fmt yuv420p output.mp4

int main(int argc, char** argv) {
  int N, iterations, type, isVerbose;
  parse_args(argc, argv, &N, &iterations, &type, &isVerbose);

  if (type == RANDOM) {
    srand(time(NULL));
  }

  Board* board = board_create(N, N);
  init_board(board, type);
  Board* nextBoard = board_create(N, N);

  for (int iter = 0; iter < iterations; iter++) {
    if (isVerbose) {
      print(board, iter);
    }

    for (int rowNr = 1; rowNr < N - 1; rowNr++) {
      evaluate_row(
          board_get_row(board, rowNr),
          board_get_row(board, rowNr - 1),
          board_get_row(board, rowNr + 1),
          N,
          board_get_row(nextBoard, rowNr)
      );
    }

    evaluate_row(
        board_get_row(board, 0),
        nullptr,
        board_get_row(board, 1),
        N,
        board_get_row(nextBoard, 0)
    );
    evaluate_row(
        board_get_row(board, N - 1),
        board_get_row(board, N - 2),
        nullptr,
        N,
        board_get_row(nextBoard, N - 1)
    );

    Board* temp = board;
    board = nextBoard;
    nextBoard = temp;
  }

  if (isVerbose) {
    print(board, iterations);
  }

  board_destroy(board);
  board_destroy(nextBoard);

  return 0;
}