#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <game_of_life/manager.h>
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
    const int verbose
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

  MPI_Request manager_request = MPI_REQUEST_NULL;

  for (int i = 0; i < iterations; i++) {
    // notify manager about current state
    if (verbose) {
      MPI_Isend(
          board->cells,
          board_size(board),
          MPI_INT32_T,
          MANAGER_ID,
          id,
          MPI_COMM_WORLD,
          &manager_request
      );
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

    // await manager request
    MPI_Wait(&manager_request, MPI_STATUS_IGNORE);

    // swap boards
    Board* temp = board;
    board = feature_board;
    feature_board = temp;
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