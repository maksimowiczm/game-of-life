#!/bin/sh
#SBATCH --output=R-%x.%j.out

srun --mpi=pmi2 ./game-of-life-main/openMP/async/async "$@"
