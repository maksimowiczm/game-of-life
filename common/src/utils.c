#include <game_of_life/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void evaluateRow(
    const bool* row,
    const bool* prevRow,
    const bool* nextRow,
    int N,
    bool* futureRow
) {
  for (int column = 0; column < N; column++) {
    int count = 0;
    for (int offset = -1; offset <= 1; offset++) {
      int newColumn = column + offset;
      if (newColumn < 0 || newColumn >= N) {
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

void WritePGM(const char* fileName, int width, int height, const bool* board) {
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

bool* initBoard(int N, enum InitType type) {
  bool* board = (bool*)malloc(N * N * sizeof(bool));
  if (board == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }

  if (type == RANDOM) {
    srand(time(NULL));
    for (int i = 0; i < N * N; i++) {
      double r = (double)rand() / RAND_MAX;
      if (r < 0.15) {
        board[i] = true;
      } else {
        board[i] = false;
      }
    }
  } else if (type == MIGRATE) {
    for (int i = 0; i < N * N; i++) {
      board[i] = false;
    }
    board[1 * N + 1] = true;
    board[2 * N + 2] = true;
    board[3 * N + 0] = true;
    board[3 * N + 1] = true;
    board[3 * N + 2] = true;
  } else if (type == HALF_PLUS) {
    for (int i = 0; i < N * N; i++) {
      board[i] = false;
    }
    board[(N / 2) * N + N / 2] = true;
    board[(N / 2) * N + 1 + N / 2] = true;
    board[(N / 2 + 1) * N + N / 2] = true;
    board[(N / 2 - 1) * N + N / 2] = true;
  }

  return board;
}

void parse_args(
    int argc,
    char* argv[],
    int* N,
    int* iterations,
    int* type,
    int* isVerbose
) {
  *N = 15;
  *iterations = 100;
  *type = HALF_PLUS;
  *isVerbose = 1;

  if (argc > 1) {
    *N = atoi(argv[1]);
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

void print(bool* board, int N, int number) {
  char fileName[50];
  mkdir("./images/", 0777);
  sprintf(fileName, "./images/life%d.pgm", number);
  WritePGM(fileName, N, N, board);
}
