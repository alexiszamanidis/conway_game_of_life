#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

// mpiexec -n 4 ./game_of_life 

int main( int argc, char **argv ) {
    int number_of_processes, rank_of_the_process;

    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimension = DEFAULT_DIMENSION, .loops = DEFAULT_LOOPS, .inputfile = DEFAULT_INPUTFILE, .output = DEFAULT_OUTPUT };
    parse_arguments(&arguments,argc,argv);

    // initialize the MPI environment
    MPI_Init(&argc, &argv);
    // get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    // get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_the_process);

    struct grid *grid = allocate_grid(arguments.dimension,number_of_processes);
    initialize_grid(&grid);

    if( rank_of_the_process == 0 ) {
        print_arguments(arguments);
        print_grid(grid,"output");
    }

    printf("Mpi rank %d out of %d processors\nis perfect square = %d\n",rank_of_the_process,number_of_processes,is_perfect_square(rank_of_the_process));

    free_grid(&grid);

    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}