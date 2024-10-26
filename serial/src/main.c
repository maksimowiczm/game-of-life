#include <game_of_life/utils.h>
#include <stdlib.h>

// ffmpeg -framerate 3 -i life%d.pgm -vf "scale=iw*10:ih*10" -c:v libx264
// -pix_fmt yuv420p output.mp4

int main(int argc, char** argv) {
  int N, iterations, type, isVerbose;
  parse_args(argc, argv, &N, &iterations, &type, &isVerbose);

  bool* board = init_board(N, type);
  bool* nextBoard = init_board(N, -1);

  for (int iter = 0; iter < iterations; iter++) {
    if (isVerbose) {
      print(board, N, iter);
    }

    for (int rowNr = 1; rowNr < N - 1; rowNr++) {
      evaluate_row(
          board + rowNr * N,
          board + (rowNr - 1) * N,
          board + (rowNr + 1) * N,
          N,
          nextBoard + rowNr * N
      );
    }
    evaluate_row(board + 0 * N, nullptr, board + 1 * N, N, nextBoard + 0 * N);
    evaluate_row(
        board + (N - 1) * N,
        board + (N - 2) * N,
        nullptr,
        N,
        nextBoard + (N - 1) * N
    );

    bool* temp = board;
    board = nextBoard;
    nextBoard = temp;
  }

  if (isVerbose) {
    print(board, N, iterations);
  }
  free(board);
  free(nextBoard);
  return 0;
}