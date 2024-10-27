#include <game_of_life/pgm.h>
#include <game_of_life/manager.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void manager_run(
    const char* output_directory,
    const int workers_count,
    const int* worker_buffer_sizes,
    const Board* board,
    const size_t iterations
) {
  const auto output_directory_length = strlen(output_directory);
  char* filename = malloc(sizeof(char) * (output_directory_length + 15));
  memset(filename, 0, sizeof(char) * (output_directory_length + 15));

  MPI_Request* promises = malloc(sizeof(MPI_Request) * workers_count);

  for (int i = 0; i < iterations; i++) {
    const auto pgm = PGM_from_board(board);
    sprintf(filename, "%s/%d.pgm", output_directory, i);

    int offset = 0;
    for (int w = 0; w < workers_count; w++) {
      for (int j = 0; j < pgm->width; j++) {
        if (pgm->buffer[offset + j] == 0) {
          pgm->buffer[offset + j] = 25;
        }
      }

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

    PGM_write(pgm, filename);
    PGM_destroy(pgm);

    // await promises
    MPI_Waitall(workers_count, promises, MPI_STATUSES_IGNORE);
  }

  free(promises);
  free(filename);
}