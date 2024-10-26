#pragma once

enum InitType { RANDOM, PLUS, MIGRATE, HALF_PLUS };

bool* initBoard(int N, enum InitType type);
void evaluateRow(
    const bool* row,
    const bool* prevRow,
    const bool* nextRow,
    int N,
    bool* futureRow
);
void WritePGM(const char* fileName, int width, int height, const bool* board);
void parse_args(
    int argc,
    char* argv[],
    int* N,
    int* iterations,
    int* type,
    int* isVerbose
);
void print(bool* board, int N, int number);
