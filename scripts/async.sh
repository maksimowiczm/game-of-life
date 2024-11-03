#!/bin/sh

srun --mpi=pmi2 ../build/async/async "$@"
