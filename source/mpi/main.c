#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

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

    if( rank_of_the_process == 0 )
        print_arguments(arguments);

    printf("Mpi rank %d out of %d processors\n",rank_of_the_process,number_of_processes);

    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}