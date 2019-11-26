#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

int main( int argc, char **argv ) {
    int number_of_processes, rank_of_the_process;

    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimension = 10, .loops = 10, .inputfile = "\0", .output = false };

    parse_arguments(&arguments,argc,argv);

    print_arguments(arguments);

    // initialize the MPI environment
    MPI_Init(&argc, &argv);
    // get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    // get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_the_process);

    printf("Mpi rank %d out of %d processors\n",rank_of_the_process,number_of_processes);

    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}