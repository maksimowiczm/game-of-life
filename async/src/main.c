#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <game_of_life/manager.h>
#include <game_of_life/utils.h>
#include <game_of_life/worker.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
  int processes_count;
  int process_id;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  Parameters parameters = {0};
  Parameters* ptr = &parameters;
  if (!parse_args(argc, argv, &ptr)) {
    MPI_Finalize();
    fprintf(stderr, "PARSER EXCEPTION");
    return 1;
  }

  char* output_directory = parameters.output_directory;
  const int width = parameters.size;
  const int height = parameters.size;
  const int iterations = parameters.iterations;
  const InitType type = parameters.type;
  const int verbose = parameters.is_verbose;

  if (processes_count < 2) {
    fprintf(stderr, "At least 2 process is required for non-verbose mode\n");
    MPI_Finalize();
    return 1;
  }

  Board* const board = board_create(width, height);

  init_board(board, type);

  const int worker_count = processes_count;
  int worker_height = height / worker_count;
  int* worker_sizes = malloc(sizeof(int) * worker_count);
  for (auto i = 0; i < worker_count; i++) {
    worker_sizes[i] = worker_height * width;
  }
  // last worker gets the remaining rows
  worker_sizes[worker_count - 1] += (height % worker_count) * width;

  if (verbose && process_id == MANAGER_ID) {
    struct stat st;
    if (mkdir(output_directory, 0777) == -1 &&
        stat(output_directory, &st) != 0) {
      fprintf(stderr, "Unable to create directory %s\n", output_directory);
    }
  }
  // run workers

  MPI_Barrier(MPI_COMM_WORLD);
  double start = MPI_Wtime();
  const int worker_id = process_id;
  // last worker gets the remaining rows
  if (worker_id == worker_count - 1) {
    worker_height += height % worker_count;
  }

  // create worker board
  Board* worker_board = board_create(width, worker_height);
  worker_get_board_part(
      board,
      worker_sizes,
      worker_id,
      worker_board->cells,
      worker_board->width * worker_board->height
  );

  WorkerType worker_type;
  if (worker_id == 0) {
    worker_type = TOP;
  } else if (worker_id == worker_count - 1) {
    worker_type = BOTTOM;
  } else {
    worker_type = MIDDLE;
  }

  worker_run(process_id, worker_type, worker_board, iterations, verbose,
       output_directory, worker_sizes);

  board_destroy(worker_board);

  MPI_Barrier(MPI_COMM_WORLD);
  double stop = MPI_Wtime();

  if (worker_id == 1)
    printf("%5.4lf\n", stop - start);
  free(worker_sizes);
  board_destroy(board);

  MPI_Finalize();
  return 0;
}
