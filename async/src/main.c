#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <manager.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <worker.h>

int main(int argc, char* argv[]) {
  // todo parse input
  // hardcoded values
  const auto width = 100;
  const auto height = 100;
  const auto verbose = true;
  const auto iterations = 400;
  const auto board = board_create(height, width);

  // todo initialize board with input
  // hardcoded values
  init_board(board, MIGRATE);

  int processes_count;
  int id;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &processes_count);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  const auto worker_count = processes_count - (verbose ? 1 : 0);
  auto worker_height = height / worker_count;

  if (id == MANAGER_ID && verbose) {
    // calculate worker sizes
    int* worker_sizes = malloc(sizeof(int) * worker_count);
    for (auto i = 0; i < worker_count; i++) {
      worker_sizes[i] = worker_height * width;
    }
    worker_sizes[worker_count - 1] += (height % worker_count) * width;

    manager_run(processes_count - 1, worker_sizes, board, iterations);

    free(worker_sizes);
    board_destroy(board);
  } else {
    // last worker gets the remaining rows
    if (id == worker_count) {
      worker_height += height % worker_count;
    }

    // create worker board
    const auto worker_board = board_create(width, worker_height);
    // copy the cells to the worker board
    memcpy(
        worker_board->cells,
        board->cells + id * worker_height * width,
        worker_height * width * sizeof(Cell)
    );
    // destroy the original board
    board_destroy(board);

    WorkerType worker_type;
    if ((!verbose && id == MANAGER_ID) || id == 1) {
      worker_type = TOP;
    } else if (id == processes_count - 1) {
      worker_type = BOTTOM;
    } else {
      worker_type = MIDDLE;
    }
    worker_run(id, worker_type, worker_board, iterations, verbose);

    board_destroy(worker_board);
  }

  MPI_Finalize();
  return 0;
}
