#include <game_of_life/board_utils.h>
#include <game_of_life/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_args(const int argc, char* argv[], Parameters** parameters) {
  memset(*parameters, 0, sizeof(Parameters));

  (*parameters)->size = 15;
  (*parameters)->iterations = 100;
  (*parameters)->type = LINE;
  (*parameters)->is_verbose = 0;

  if (argc < 3) {
    printf(
        "Usage: %s <size> <iterations> <type> <output_directory>\n",
        argv[0]
    );
    printf("  size: size of the board\n");
    printf("  iterations: number of iterations\n");
    printf("  type: type of the initial board\n");
    printf("    0: random\n");
    printf("    1: small_line\n");
    printf("    2: migrate\n");
    printf("    3: half_plus\n");
    printf("    4: line\n");
    printf("  output_directory: directory to save the output\n");
    return 0;
  }

  (*parameters)->size = atoi(argv[1]);
  (*parameters)->iterations = atoi(argv[2]);

  if (argc > 3) {
    (*parameters)->type = atoi(argv[3]);
  }
  if (argc > 4) {
    (*parameters)->is_verbose = 1;
    memcpy((*parameters)->output_directory, argv[4], strlen(argv[4]));
  }

  return 1;
}
