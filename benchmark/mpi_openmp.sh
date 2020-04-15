#!/bin/bash

function print_green {
    reset='\033[0m'             # reset
    green='\033[0;32m'          # green
    echo -e "${green}$1${reset}"
}

clear

# compile
cd ../source && make mpi_openmp 2>&1 > /dev/null

# run benchmarks
print_green "Processors: 1, Dimension: 840, Generations: 100, Threads: 2"
mpiexec -n 1 ./mpi_openmp -d 840 -g 100 -t 2
print_green "Processors: 1, Dimension: 1680, Generations: 100, Threads: 2"
mpiexec -n 1 ./mpi_openmp -d 1680 -g 100 -t 2
print_green "Processors: 1, Dimension: 3360, Generations: 100, Threads: 2"
mpiexec -n 1 ./mpi_openmp -d 3360 -g 100 -t 2
print_green "Processors: 1, Dimension: 6720, Generations: 100, Threads: 2"
mpiexec -n 1 ./mpi_openmp -d 6720 -g 100 -t 2
###########################################################
print_green "Processors: 4, Dimension: 840, Generations: 100, Threads: 2"
mpiexec -n 4 ./mpi_openmp -d 840 -g 100 -t 2
print_green "Processors: 4, Dimension: 1680, Generations: 100, Threads: 2"
mpiexec -n 4 ./mpi_openmp -d 1680 -g 100 -t 2
print_green "Processors: 4, Dimension: 3360, Generations: 100, Threads: 2"
mpiexec -n 4 ./mpi_openmp -d 3360 -g 100 -t 2
print_green "Processors: 4, Dimension: 6720, Generations: 100, Threads: 2"
mpiexec -n 4 ./mpi_openmp -d 6720 -g 100 -t 2
###########################################################
print_green "Processors: 9, Dimension: 840, Generations: 100, Threads: 2"
mpiexec -n 9 ./mpi_openmp -d 840 -g 100 -t 2
print_green "Processors: 9, Dimension: 1680, Generations: 100, Threads: 2"
mpiexec -n 9 ./mpi_openmp -d 1680 -g 100 -t 2
print_green "Processors: 9, Dimension: 3360, Generations: 100, Threads: 2"
mpiexec -n 9 ./mpi_openmp -d 3360 -g 100 -t 2
print_green "Processors: 9, Dimension: 6720, Generations: 100, Threads: 2"
mpiexec -n 9 ./mpi_openmp -d 6720 -g 100 -t 2
###########################################################

# clean
make clean 2>&1 > /dev/null