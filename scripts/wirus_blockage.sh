#!/bin/bash

sbatch -n "$1" ./blockage.sh "${@:2}"