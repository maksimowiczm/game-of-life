#include <game_of_life/utils.h>
#include <manager.h>
#include <mpi.h>
#include <stdlib.h>

void manager_run(
    const int workers_count,
    const int* worker_buffer_sizes,
    const Board* board,
    const size_t iterations
) {
  MPI_Request* promises = malloc(sizeof(MPI_Request) * workers_count);

  for (int i = 0; i < iterations; i++) {
    int offset = 0;
    for (int w = 0; w < workers_count; w++) {
      const auto worker_id = w + 1;
      MPI_Irecv(
          board->cells + offset,
          worker_buffer_sizes[w],
          MPI_INT32_T,
          worker_id,
          worker_id,
          MPI_COMM_WORLD,
          &promises[w]
      );

      offset += worker_buffer_sizes[w];
    }

    // await promises
    MPI_Waitall(workers_count, promises, MPI_STATUSES_IGNORE);

    print(board, i);
  }

  free(promises);
}