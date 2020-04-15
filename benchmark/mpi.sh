#!/bin/bash

function print_green {
    reset='\033[0m'             # reset
    green='\033[0;32m'          # green
    echo -e "${green}$1${reset}"
}

clear

# compile
cd ../source && make mpi 2>&1 > /dev/null

# run benchmarks
print_green "Processors: 1, Dimension: 840, Generations: 100"
mpiexec -n 1 ./mpi -d 840 -g 100
print_green "Processors: 1, Dimension: 1680, Generations: 100"
mpiexec -n 1 ./mpi -d 1680 -g 100
print_green "Processors: 1, Dimension: 3360, Generations: 100"
mpiexec -n 1 ./mpi -d 3360 -g 100
print_green "Processors: 1, Dimension: 6720, Generations: 100"
mpiexec -n 1 ./mpi -d 6720 -g 100
###########################################################
print_green "Processors: 4, Dimension: 840, Generations: 100"
mpiexec -n 4 ./mpi -d 840 -g 100
print_green "Processors: 4, Dimension: 1680, Generations: 100"
mpiexec -n 4 ./mpi -d 1680 -g 100
print_green "Processors: 4, Dimension: 3360, Generations: 100"
mpiexec -n 4 ./mpi -d 3360 -g 100
print_green "Processors: 4, Dimension: 6720, Generations: 100"
mpiexec -n 4 ./mpi -d 6720 -g 100
###########################################################
print_green "Processors: 9, Dimension: 840, Generations: 100"
mpiexec -n 9 ./mpi -d 840 -g 100
print_green "Processors: 9, Dimension: 1680, Generations: 100"
mpiexec -n 9 ./mpi -d 1680 -g 100
print_green "Processors: 9, Dimension: 3360, Generations: 100"
mpiexec -n 9 ./mpi -d 3360 -g 100
print_green "Processors: 9, Dimension: 6720, Generations: 100"
mpiexec -n 9 ./mpi -d 6720 -g 100
###########################################################

# clean
make clean 2>&1 > /dev/null