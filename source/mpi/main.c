#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

// mpiexec -n 4 ./game_of_life 

int main( int argc, char **argv ) {
    int number_of_processes, rank_of_the_process;
    double local_start, local_end, local_elapsed, max_elapsed;

    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimension = DEFAULT_DIMENSION, .loops = DEFAULT_LOOPS, .inputfile = DEFAULT_INPUTFILE, .output = DEFAULT_OUTPUT };
    parse_arguments(&arguments,argc,argv);

    // initialize the MPI environment, and disable Profiling
    MPI_Init(&argc, &argv);
    MPI_Pcontrol(0);
    // get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    // get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_the_process);

    // start Wtime and Profiling
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    MPI_Pcontrol(1);

    struct grid *grid = allocate_grid(arguments.dimension,number_of_processes);
    initialize_grid(&grid);

    if( rank_of_the_process == 0 ) {
        print_arguments(arguments);
        print_grid(grid,"output");
    }

    printf("Mpi rank %d out of %d processors\nis perfect square = %d\n",rank_of_the_process,number_of_processes,is_perfect_square(rank_of_the_process));

    // stop Wtime and Profiling
    local_end = MPI_Wtime();
    MPI_Pcontrol(0);
    // calculate local elapsed time
    local_elapsed = local_end - local_start;
    // get elapsed time by each process and find the max elapsed time
    MPI_Reduce(&local_elapsed,&max_elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

    if( rank_of_the_process == 0 )
        printf("Elapsed time = %f\n",max_elapsed);

    free_grid(&grid);
    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}