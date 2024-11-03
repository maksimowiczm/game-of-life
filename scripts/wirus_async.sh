#!/bin/bash

sbatch -n "$1" -c "$2" ./async.sh "${@:3}"
