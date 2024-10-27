#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <manager.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <worker.h>

int main(int argc, char* argv[]) {
  int processes_count;
  int process_id;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

  // todo parse input
  // hardcoded values
  const auto width = 100;
  const auto height = 100;
  const auto verbose = true;
  const auto iterations = 400;

  if (verbose && processes_count < 3) {
    fprintf(stderr, "At least 3 processes are required for verbose mode\n");
    MPI_Finalize();
    return 1;
  }
  if (!verbose && processes_count < 2) {
    fprintf(stderr, "At least 2 process is required for non-verbose mode\n");
    MPI_Finalize();
    return 1;
  }

  Board* const board = board_create(height, width);

  // todo initialize board with input
  // hardcoded values
  init_board(board, MIGRATE);

  const int worker_count = verbose ? processes_count - 1 : processes_count;
  int worker_height = height / worker_count;

  // run manager
  if (verbose && process_id == MANAGER_ID) {
    if (process_id == MANAGER_ID) {
      int* worker_sizes = malloc(sizeof(int) * worker_count);
      for (auto i = 0; i < worker_count; i++) {
        worker_sizes[i] = worker_height * width;
      }
      // last worker gets the remaining rows
      worker_sizes[worker_count - 1] += (height % worker_count) * width;

      manager_run(processes_count - 1, worker_sizes, board, iterations);

      free(worker_sizes);
      board_destroy(board);
    }
  }
  // run workers
  else {
    const int worker_id = verbose ? process_id - 1 : process_id;
    // last worker gets the remaining rows
    if (worker_id == worker_count) {
      worker_height += height % worker_count;
    }

    // create worker board
    Board* worker_board = board_create(width, worker_height);
    const int worker_board_start = worker_id * worker_height * width;
    // copy the cells to the worker board
    memcpy(
        worker_board->cells,
        board->cells + worker_board_start,
        worker_height * width * sizeof(Cell)
    );
    // destroy the original board
    board_destroy(board);

    WorkerType worker_type;
    if (worker_id == 0) {
      worker_type = TOP;
    } else if (worker_id == worker_count - 1) {
      worker_type = BOTTOM;
    } else {
      worker_type = MIDDLE;
    }

    worker_run(process_id, worker_type, worker_board, iterations, verbose);

    board_destroy(worker_board);
  }

  MPI_Finalize();
  return 0;
}
