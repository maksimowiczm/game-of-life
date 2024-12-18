#!/bin/bash
#SBATCH --output=wirus.out


if [ $# -lt 2 ]; then
  echo "Użycie: $0 <liczba zadań> <liczba rdzeni>"
  exit 1
fi

tasks=$(( $1 * $2 ))
sbatch -n "$tasks" -c 1 ./async.sh "${@:3}"
sbatch -n "$tasks" -c 1 ./async.sh "${@:3}"
sbatch -n "$tasks" -c 1 ./async.sh "${@:3}"

sbatch -n "$1" -c "$2" ./async_omp.sh "${@:3}"
sbatch -n "$1" -c "$2" ./async_omp.sh "${@:3}"
sbatch -n "$1" -c "$2" ./async_omp.sh "${@:3}"

sbatch -n "$tasks" -c 1 ./blockage.sh "${@:3}"
sbatch -n "$tasks" -c 1 ./blockage.sh "${@:3}"
sbatch -n "$tasks" -c 1 ./blockage.sh "${@:3}"

sbatch -n "$1" -c "$2" ./blockage_omp.sh "${@:3}"
sbatch -n "$1" -c "$2" ./blockage_omp.sh "${@:3}"
sbatch -n "$1" -c "$2" ./blockage_omp.sh "${@:3}"
