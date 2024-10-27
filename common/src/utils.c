#include <game_of_life/board_utils.h>
#include <game_of_life/utils.h>
#include <stdlib.h>

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
