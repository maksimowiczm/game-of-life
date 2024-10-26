#include <game_of_life/board.h>
#include <game_of_life/board_utils.h>
#include <manager.h>
#include <mpi.h>
#include <stdlib.h>
#include <worker.h>

void worker_run(
    const int id,
    const WorkerType worker_type,
    const Board* board,
    const size_t iterations,
    const bool verbose
) {
  const Board* feature_board = board_create(board->width, board->height);
  Cell* ghost_top = nullptr;
  if (worker_type != TOP) {
    ghost_top = malloc(sizeof(Cell) * board->width);
  }

  Cell* ghost_bottom = nullptr;
  if (worker_type != BOTTOM) {
    ghost_bottom = malloc(sizeof(Cell) * board->width);
  }

  MPI_Request send_requests[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
  MPI_Request edge_requests[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
  MPI_Request manager_request = MPI_REQUEST_NULL;

  for (auto i = 0; i < iterations; i++) {
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
    for (auto row = 1; row < board->height - 1; row++) {
      const auto previous_row = board_get_row(board, row - 1);
      const auto current_row = board_get_row(board, row);
      const auto next_row = board_get_row(board, row + 1);

      evaluate_row(
          current_row,
          previous_row,
          next_row,
          board->width,
          board_get_row(feature_board, row)
      );
    }

    // receive edge rows
    if (ghost_top != nullptr) {
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
    if (ghost_bottom != nullptr) {
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
    MPI_Waitall(4, promises, MPI_STATUSES_IGNORE);

    // evaluate edge rows
    if (ghost_top != nullptr) {
      evaluate_row(
          board_get_row(board, 0),
          ghost_top,
          board_get_row(board, 1),
          board->width,
          board_get_row(feature_board, 0)
      );
    }
    if (ghost_bottom != nullptr) {
      evaluate_row(
          board_get_row(board, board->height - 1),
          board_get_row(board, board->height - 2),
          ghost_bottom,
          board->width,
          board_get_row(feature_board, board->height - 1)
      );
    }

    // await manager request
    MPI_Wait(&manager_request, MPI_STATUS_IGNORE);

    // swap boards
    const Board* temp = board;
    board = feature_board;
    feature_board = temp;
  }

  free(ghost_top);
  free(ghost_bottom);
}