#include <game_of_life/pgm.h>
#include <stdio.h>
#include <stdlib.h>

void PGM_write(const PGM* const pgm, const char* const file_name) {
  FILE* file = fopen(file_name, "wb");
  if (file == NULL) {
    fprintf(stderr, "Unable to open file %s\n", file_name);
    return;
  }

  fprintf(file, "P5\n%lu %lu\n255\n", pgm->width, pgm->height);

  fwrite(pgm->buffer, sizeof(u_int8_t), pgm->width * pgm->height, file);

  fclose(file);
}

PGM* PGM_from_board(const Board* const board) {
  PGM* pgm = (PGM*)malloc(sizeof(PGM));

  pgm->width = board->width;
  pgm->height = board->height;
  pgm->buffer = (u_int8_t*)malloc(board_size(board) * sizeof(u_int8_t));

  for (int i = 0; i < board_size(board); i++) {
    pgm->buffer[i] = board->cells[i] == ALIVE ? 255 : 0;
  }

  return pgm;
}

void PGM_destroy(PGM* pgm) {
  free(pgm->buffer);
  free(pgm);
}