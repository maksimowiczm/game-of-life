#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <game_of_life/manager.h>
#include <game_of_life/worker.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  MPI_Request send_requests[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
  MPI_Request edge_requests[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
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

    // notify other workers with our edge rows
    if (worker_type != TOP) {
      MPI_Isend(
          board_get_row(board, 0),
          board->width,
          MPI_INT32_T,
          id - 1,
          id,
          MPI_COMM_WORLD,
          &send_requests[0]
      );
    }
    if (worker_type != BOTTOM) {
      MPI_Isend(
          board_get_row(board, board->height - 1),
          board->width,
          MPI_INT32_T,
          id + 1,
          id,
          MPI_COMM_WORLD,
          &send_requests[1]
      );
    }

    // evaluate inner rows
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

    // receive edge rows
    if (ghost_top != NULL) {
      MPI_Irecv(
          ghost_top,
          board->width,
          MPI_INT32_T,
          id - 1,
          id - 1,
          MPI_COMM_WORLD,
          &edge_requests[0]
      );
    }
    if (ghost_bottom != NULL) {
      MPI_Irecv(
          ghost_bottom,
          board->width,
          MPI_INT32_T,
          id + 1,
          id + 1,
          MPI_COMM_WORLD,
          &edge_requests[1]
      );
    }

    // await promises
    MPI_Request promises[4] = {
        send_requests[0],
        send_requests[1],
        edge_requests[0],
        edge_requests[1]
    };

    MPI_Status statuses[4];
    MPI_Waitall(4, promises, statuses);

    for(int i = 0; i < 4; i++) {
      if(statuses[i].MPI_ERROR != MPI_SUCCESS) {
        fprintf(stderr, "wait all error");
      }
    }

    // evaluate edge rows
    evaluate_row(
        board_get_row(board, 0),
        ghost_top,
        board_get_row(board, 1),
        board->width,
        board_get_row(feature_board, 0)
    );

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
    fprintf(stderr, "Worker %d buffer size is too small\n", worker_id);
    return;
  }

  memcpy(
      worker_buffer,
      board->cells + offset,
      worker_buffer_size * sizeof(Cell)
  );
}