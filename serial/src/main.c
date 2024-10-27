#include <game_of_life/board_utils.h>
#include <game_of_life/pgm.h>
#include <game_of_life/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

// ffmpeg -framerate 3 -i life%d.pgm -vf "scale=iw*10:ih*10" -c:v libx264
// -pix_fmt yuv420p output.mp4

void print(
    const Board* const board,
    const char* output_directory,
    const int32_t iteration
) {
  char file_name[1000 + 100] = {0};
  sprintf(file_name, "./%s/%d.pgm", output_directory, iteration);

  const auto pgm = PGM_from_board(board);
  PGM_write(pgm, file_name);
  PGM_destroy(pgm);
}

int main(int argc, char** argv) {
  Parameters parameters = {0};
  auto ptr = &parameters;
  if (!parse_args(argc, argv, &ptr)) {
    return 0;
  }

  const auto output_directory = parameters.output_directory;
  const auto N = parameters.size;
  const auto iterations = parameters.iterations;
  const auto type = parameters.type;
  const auto isVerbose = parameters.is_verbose;

  if (isVerbose && mkdir(output_directory, 0777) == -1) {
    fprintf(stderr, "Unable to create directory %s\n", output_directory);
    return 1;
  }

  if (type == RANDOM) {
    srand(time(NULL));
  }

  Board* board = board_create(N, N);
  init_board(board, type);
  Board* nextBoard = board_create(N, N);

  for (int iter = 0; iter < iterations; iter++) {
    if (isVerbose) {
      print(board, output_directory, iter);
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
    print(board, output_directory, iterations);
  }

  board_destroy(board);
  board_destroy(nextBoard);

  return 0;
}