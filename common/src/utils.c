#include <game_of_life/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void evaluate_row(
    const bool* row,
    const bool* prevRow,
    const bool* nextRow,
    size_t size,
    bool* futureRow
) {
  for (int column = 0; column < size; column++) {
    int count = 0;
    for (int offset = -1; offset <= 1; offset++) {
      int newColumn = column + offset;
      if (newColumn < 0 || newColumn >= size) {
        continue;
      }
      if (prevRow && prevRow[newColumn]) {
        count++;
      }
      if (nextRow && nextRow[newColumn]) {
        count++;
      }
      if (offset == 0) {
        continue;
      }
      if (row[newColumn]) {
        count++;
      }
    }
    if (row[column]) {
      futureRow[column] = (count == 2 || count == 3);
    } else {
      futureRow[column] = (count == 3);
    }
  }
}

void write_pgm(const char* fileName, int width, int height, const bool* board) {
  FILE* file = fopen(fileName, "wb");
  if (file == NULL) {
    printf("Unable to open file %s\n", fileName);
    return;
  }

  fprintf(file, "P5\n%d %d\n255\n", width, height);

  unsigned char* buffer =
      (unsigned char*)malloc(width * height * sizeof(unsigned char));
  if (buffer == NULL) {
    fclose(file);
    return;
  }

  for (int i = 0; i < width * height; i++) {
    buffer[i] = board[i] ? 255 : 0;
  }

  fwrite(buffer, sizeof(unsigned char), width * height, file);

  free(buffer);
  fclose(file);
}

bool* init_board(size_t size, enum InitType type) {
  bool* board = (bool*)malloc(size * size * sizeof(bool));
  if (board == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }

  if (type == RANDOM) {
    srand(time(NULL));
    for (int i = 0; i < size * size; i++) {
      double r = (double)rand() / RAND_MAX;
      if (r < 0.15) {
        board[i] = true;
      } else {
        board[i] = false;
      }
    }
  } else if (type == MIGRATE) {
    for (int i = 0; i < size * size; i++) {
      board[i] = false;
    }
    board[1 * size + 1] = true;
    board[2 * size + 2] = true;
    board[3 * size + 0] = true;
    board[3 * size + 1] = true;
    board[3 * size + 2] = true;
  } else if (type == HALF_PLUS) {
    for (int i = 0; i < size * size; i++) {
      board[i] = false;
    }
    board[(size / 2) * size + size / 2] = true;
    board[(size / 2) * size + 1 + size / 2] = true;
    board[(size / 2 + 1) * size + size / 2] = true;
    board[(size / 2 - 1) * size + size / 2] = true;
  }

  return board;
}

void parse_args(
    int argc,
    char* argv[],
    int* size,
    int* iterations,
    int* type,
    int* isVerbose
) {
  *size = 15;
  *iterations = 100;
  *type = HALF_PLUS;
  *isVerbose = 1;

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
    *isVerbose = atoi(argv[4]);
  }
}

void print(const bool* board, size_t size, int number) {
  char fileName[50];
  mkdir("./images/", 0777);
  sprintf(fileName, "./images/life%d.pgm", number);
  write_pgm(fileName, size, size, board);
}
