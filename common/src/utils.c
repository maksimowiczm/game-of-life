#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <game_of_life/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void write_pgm(const char* const file_name, const Board* const board) {
  FILE* file = fopen(file_name, "wb");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file %s\n", file_name);
    return;
  }

  fprintf(file, "P5\n%lu %lu\n255\n", board->width, board->height);

  const auto buffer =
      (unsigned char*)malloc(board_size(board) * sizeof(u_int8_t));

  if (buffer == nullptr) {
    fprintf(stderr, "Unable to allocate memory for PGM file\n");
    fclose(file);
    return;
  }

  for (int i = 0; i < board_size(board); i++) {
    buffer[i] = board->cells[i] == ALIVE ? 255 : 0;
  }

  fwrite(buffer, sizeof(u_int8_t), board_size(board), file);

  free(buffer);
  fclose(file);
}

void parse_args(
    int argc,
    char* argv[],
    int* size,
    int* iterations,
    int* type,
    int* is_verbose
) {
  *size = 15;
  *iterations = 100;
  *type = HALF_PLUS;
  *is_verbose = 1;

  if (argc > 1) {
    *size = atoi(argv[1]);
  }
  if (argc > 2) {
    *iterations = atoi(argv[2]);
  }
  if (argc > 3) {
    *type = atoi(argv[3]);
  }
  if (argc > 4) {
    *is_verbose = atoi(argv[4]);
  }
}

void print(const Board* const board, const int32_t number) {
  char file_name[19 + 10 + 1] = {0};
  // yes, it ignores mkdir return value :(
  mkdir("./images/", 0777);
  sprintf(file_name, "./images/life%d.pgm", number);
  write_pgm(file_name, board);
}
