#!/bin/sh

srun --mpi=pmi2 ../build/blockage/blockage "$@"
