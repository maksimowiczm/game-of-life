#pragma once

#include <game_of_life/board.h>
#include <stddef.h>
#include <sys/types.h>

struct PGM {
  size_t width;
  size_t height;
  u_int8_t* buffer;
};

typedef struct PGM PGM;

void PGM_write(const PGM* pgm, const char* file_name);

PGM* PGM_from_board(const Board* board);

void PGM_destroy(PGM* pgm);