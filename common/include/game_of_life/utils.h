#pragma once

struct parameters {
  int size;
  int iterations;
  InitType type;
  bool is_verbose;
  char output_directory[1000];
};

typedef struct parameters Parameters;

bool parse_args(int argc, char* argv[], Parameters** parameters);
