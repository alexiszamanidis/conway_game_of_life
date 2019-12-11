#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

// mpiexec -n 4 ./game_of_life -d 20 -i ../inputfiles/grid_20x20.csv -o

int main( int argc, char **argv ) {
    int number_of_processes, rank_of_the_process;
    double local_start, local_end, local_elapsed, max_elapsed;
    char **local_grid, **next_local_grid;

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

    // allocate and initialize grid
    struct grid *grid = allocate_grid(arguments.dimension,number_of_processes);
    if( strcmp(arguments.inputfile,"") == 0 )
        initialize_grid(&grid);
    else
        initialize_grid_from_inputfile(&grid,arguments.inputfile);

    if( (rank_of_the_process == 0) && (arguments.output == true) ) {
        print_arguments(arguments);
        print_grid(grid,"output.csv");
    }

    // allocate local grid and next local grid
    local_grid = allocate_2d_array(grid->subgrid_dimension);
    next_local_grid = allocate_2d_array(grid->subgrid_dimension);

    // stop Wtime and Profiling
    local_end = MPI_Wtime();
    MPI_Pcontrol(0);
    // calculate local elapsed time
    local_elapsed = local_end - local_start;
    // get elapsed time by each process and find the max elapsed time
    MPI_Reduce(&local_elapsed,&max_elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

    if( rank_of_the_process == 0 )
        printf("Elapsed time = %f\n",max_elapsed);

    
    free_2d_array(local_grid,grid->subgrid_dimension);
    free_2d_array(next_local_grid,grid->subgrid_dimension);
    free_grid(&grid);
    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}