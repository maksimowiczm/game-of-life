#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <game_of_life/manager.h>
#include <game_of_life/pgm.h>
#include <game_of_life/worker.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _OPENMP
#include <omp.h>
#endif

void worker_run(
    const int id,
    const WorkerType worker_type,
    Board* board,
    const size_t iterations,
    const int verbose,
    char* output_directory,
    int* worker_sizes
) {
  Board* feature_board = board_create(board->width, board->height);
  Cell* ghost_top = NULL;
  if (worker_type != TOP) {
    ghost_top = malloc(sizeof(Cell) * board->width);
  }

  Cell* ghost_bottom = NULL;
  if (worker_type != BOTTOM) {
    ghost_bottom = malloc(sizeof(Cell) * board->width);
  }

  int total_recv_size;
  int* displs;
  int num_processes;
  char* filename;
  Board* out_board;
  if (verbose) {
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    displs = malloc(sizeof(int) * num_processes);
    total_recv_size = 0;
    for (int proc = 0; proc < num_processes; proc++) {
      displs[proc] = total_recv_size;
      total_recv_size += worker_sizes[proc];
    }
    if (id == MANAGER_ID) {
      out_board = board_create(board->width, total_recv_size / board->width);
      const int output_directory_length = strlen(output_directory);
      filename = malloc(sizeof(char) * (output_directory_length + 15));
      memset(filename, 0, sizeof(char) * (output_directory_length + 15));
    }

  }

  for (int i = 0; i < iterations; i++) {
    if (verbose) {
      int err = MPI_Gatherv(
        board->cells,
        board->width * board->height,
        MPI_INT32_T,
        out_board ? out_board->cells : NULL,
        worker_sizes,
        displs,
        MPI_INT32_T,
        MANAGER_ID,
        MPI_COMM_WORLD
      );
      if (err != MPI_SUCCESS) {
        fprintf(stderr, "MPI_Gatherv failed");
        MPI_Abort(MPI_COMM_WORLD, err);
      }

      if (id == MANAGER_ID) {
        PGM* pgm = PGM_from_board(out_board);
        sprintf(filename, "%s/%d.pgm", output_directory, i);
        PGM_write(pgm, filename);
        PGM_destroy(pgm);
      }
    }

    // communication between workers
    if (worker_type != TOP && ghost_top != NULL) {
      int error_code;
      error_code = MPI_Sendrecv(
          board_get_row(board, 0),
          board->width,
          MPI_INT32_T,
          id - 1,
          id,
          ghost_top,
          board->width,
          MPI_INT32_T,
          id - 1,
          id - 1,
          MPI_COMM_WORLD,
          MPI_STATUS_IGNORE
      );
      if(error_code != MPI_SUCCESS){
        printf("stderr: MPI FAILED 1\n");
      }

    }
    if (worker_type != BOTTOM && ghost_bottom != NULL) {
      int error_code;
      error_code = MPI_Sendrecv(
        board_get_row(board, board->height - 1),
        board->width,
        MPI_INT32_T,
        id + 1,
        id,
        ghost_bottom,
        board->width,
        MPI_INT32_T,
        id + 1,
        id + 1,
        MPI_COMM_WORLD,
        MPI_STATUS_IGNORE
    );
    if(error_code != MPI_SUCCESS){
        printf("stderr: MPI FAILED 2\n");
      }
    }

    // evaluate top row
    evaluate_row(
        board_get_row(board, 0),
        ghost_top,
        board_get_row(board, 1),
        board->width,
        board_get_row(feature_board, 0)
    );

    // evaluate inner rows using OpenMP
    #pragma omp parallel for
    for (int row = 1; row < board->height - 1; row++) {
      const Cell* const previous_row = board_get_row(board, row - 1);
      const Cell* const current_row = board_get_row(board, row);
      const Cell* const next_row = board_get_row(board, row + 1);

      evaluate_row(
          current_row,
          previous_row,
          next_row,
          board->width,
          board_get_row(feature_board, row)
      );
    }

    evaluate_row(
        board_get_row(board, board->height - 1),
        board_get_row(board, board->height - 2),
        ghost_bottom,
        board->width,
        board_get_row(feature_board, board->height - 1)
    );

    // swap boards
    Board* temp = board;
    board = feature_board;
    feature_board = temp;
  }

  if (verbose) {
    free(displs);
    if (id == MANAGER_ID) {
      board_destroy(out_board);
      free(filename);
    }
  }

  board_destroy(feature_board);
  free(ghost_top);
  free(ghost_bottom);
}

void worker_get_board_part(
    const Board* board,
    const int* worker_buffer_sizes,
    const int worker_id,
    Cell* worker_buffer,
    const int worker_buffer_size
) {
  int offset = 0;
  for (int i = 0; i < worker_id; i++) {
    offset += worker_buffer_sizes[i];
  }

  if (worker_buffer_size < worker_buffer_sizes[worker_id]) {
    fprintf(stderr, "Worker buffer size is too small\n");
    return;
  }

  memcpy(
      worker_buffer,
      board->cells + offset,
      worker_buffer_size * sizeof(Cell)
  );
}