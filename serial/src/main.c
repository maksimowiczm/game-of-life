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

  PGM* pgm = PGM_from_board(board);
  PGM_write(pgm, file_name);
  PGM_destroy(pgm);
}

int main(int argc, char** argv) {
  Parameters parameters = {0};
  Parameters* ptr = &parameters;
  if (!parse_args(argc, argv, &ptr)) {
    return 0;
  }

  char* output_directory = parameters.output_directory;
  const int width = parameters.size;
  const int height = parameters.size;
  const int iterations = parameters.iterations;
  const InitType type = parameters.type;
  const int verbose = parameters.is_verbose;
  const int N = width;

  if (verbose && mkdir(output_directory, 0777) == -1) {
    fprintf(stderr, "Unable to create directory %s\n", output_directory);
    return 1;
  }

  if (type == RANDOM) {
    srand(time(NULL));
  }

  Board* board = board_create(N, N);
  init_board(board, type);
  Board* nextBoard = board_create(N, N);

  struct timespec tp1,tp2;
  clock_gettime(CLOCK_MONOTONIC,&tp1);

  for (int iter = 0; iter < iterations; iter++) {
    if (verbose) {
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
        NULL,
        board_get_row(board, 1),
        N,
        board_get_row(nextBoard, 0)
    );
    evaluate_row(
        board_get_row(board, N - 1),
        board_get_row(board, N - 2),
        NULL,
        N,
        board_get_row(nextBoard, N - 1)
    );

    Board* temp = board;
    board = nextBoard;
    nextBoard = temp;
  }
  clock_gettime(CLOCK_MONOTONIC,&tp2);

  double extime=(double)tp2.tv_sec+1e-9*(double)tp2.tv_nsec-(double)tp1.tv_sec-1e-9*(double)tp1.tv_nsec;
  printf("Execution time: %5.3f ms\n",extime*1000.0);

  if (verbose) {
    print(board, output_directory, iterations);
  }

  board_destroy(board);
  board_destroy(nextBoard);

  return 0;
}