#include "../header/header.h"
#include "../header/grid.h"
#include "../header/utilities.h"
#include "../header/mpi.h"

// mpiexec -n 4 ./mpi -d 10 -g 2 -i ../inputfiles/grid_10x10.csv

int main( int argc, char **argv ) {
    double local_start, local_end, local_elapsed, max_elapsed;
    int number_of_processes, rank_of_the_process, generation, generation_continue = 1, different_generations, sum_different_generations = 0;
    int last, subgrid_dimension, process_grid_dimension, *sendcounts, *displs;
    struct grid *grid = NULL, *current_generation = NULL, *next_generation = NULL;
    struct neighbour_processes neighbour_processes;
    struct grid_side_dimensions *grid_side_dimensions = NULL;
    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimension = DEFAULT_DIMENSION, .generations = DEFAULT_GENERATIONS, .inputfile = DEFAULT_INPUTFILE, .threads = 0 };
    parse_arguments(&arguments,argc,argv);

    // initialize the MPI environment, and disable Profiling
    MPI_Init(&argc, &argv);
    MPI_Pcontrol(0);
    // get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    // get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_the_process);

    MPI_Datatype blocktype_1, blocktype_2;
    MPI_Status status[16];
    MPI_Request *request = my_malloc(MPI_Request,16);
    MPI_Datatype columns;

    // allocate, initialize grid and different generations array
    grid = allocate_grid(arguments.dimension);
    if( rank_of_the_process == 0 ) {
        if( strcmp(arguments.inputfile,"") == 0 )
            initialize_grid(&grid);
        else
            initialize_grid_from_inputfile(&grid,arguments.inputfile);
    }
    // caclulate subgrid dimension and process grid dimension
    subgrid_dimension = calculate_subgrid_dimension(arguments.dimension,number_of_processes);
    process_grid_dimension = arguments.dimension/subgrid_dimension;
    // allocate grid side dimensions,local grid, next local grid and initialize dim for mpi cart
    grid_side_dimensions = allocate_grid_side_dimensions(subgrid_dimension);
    current_generation = allocate_grid(subgrid_dimension);
    next_generation = allocate_grid(subgrid_dimension);
    last = subgrid_dimension-1;

    // mpi partition grid into blocks
    MPI_Type_vector(subgrid_dimension, subgrid_dimension, grid->dimension, MPI_CHAR, &blocktype_2);
    MPI_Type_create_resized(blocktype_2, 0, sizeof(char), &blocktype_1);
    MPI_Type_commit(&blocktype_1);
    // allocate sendcounts and displacements
    sendcounts = (int *)malloc(sizeof(int)*((int)pow(process_grid_dimension,2)));
    displs = (int *)malloc(sizeof(int)*((int)pow(process_grid_dimension,2)));
    // initialize sendcounts, displacements and scatter the grid
    initialize_sendcounts_and_displs_for_scattering_the_grid(sendcounts,displs,grid->dimension,subgrid_dimension,process_grid_dimension);

    // scatter a buffer in blocks to all processes in a communicator  
    MPI_Scatterv(&(grid->array[0][0]), sendcounts, displs, blocktype_1, &current_generation->array[0][0], subgrid_dimension*subgrid_dimension, MPI_CHAR, 0, MPI_COMM_WORLD);

    // calculate neighbour coordinates and ranks
    calculate_neighbours(&neighbour_processes,rank_of_the_process,process_grid_dimension);

    MPI_Type_vector(current_generation->dimension, 1, current_generation->dimension, MPI_CHAR, &columns);
    MPI_Type_commit(&columns);

#if GOL_DEBUG
    // print arguments and global grid if the user compiled with -DGOL_DEBUG option for printing/debugging
    if( rank_of_the_process == 0 ) {
        print_arguments(arguments);
        print_grid(grid, rank_of_the_process, "global_grid", 0);
    }
#endif

    // start Wtime and Profiling
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    MPI_Pcontrol(1);

    // basic structure of central iteration:
    for( generation = 0 ; (generation < arguments.generations) && (generation_continue == 1) ; generation++ ) {
        
        // receive all neighbours
        receive(grid_side_dimensions,current_generation,neighbour_processes,request);
        // send all neighbours
        send(current_generation,neighbour_processes,columns,request);
        
        different_generations = 0;

        // calculate intermidiate elements while waiting for receiving the outlines
        calculate_intermidiate_elements(current_generation,next_generation,&different_generations);

        // wait for all given communications to complete
        MPI_Waitall(16, request, status);   

        // when we have received outline elements from other processes we calculate them
        calculate_outline_elements(current_generation,next_generation,grid_side_dimensions,&different_generations,last);

        // gather global global grid, print current generation and next generation grid for each process if the user compiled with -DGOL_DEBUG option for printing/debugging
#if GOL_DEBUG
        MPI_Gatherv(&(next_generation->array[0][0]), next_generation->dimension * next_generation->dimension, MPI_CHAR, &(grid->array[0][0]), sendcounts, displs, blocktype_1, 0,MPI_COMM_WORLD);
        print_grid(current_generation, rank_of_the_process, "current_generation", generation+1);
        print_grid(next_generation, rank_of_the_process, "next_generation", generation+1);
        // if the process's rank is 0, then print current generation to stdout and global grid
        if( rank_of_the_process == 0 ) {
            printf("Generation: %d\n", generation+1);
            print_grid(grid, rank_of_the_process, "global_grid", generation+1);
        }
#endif
        // swap generations
        swap(current_generation,next_generation);

        // get all different generations variables
        MPI_Reduce(&different_generations, &sum_different_generations, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        // if there is at least one generation from all the grids that changed, then continue the generation loop
        if( rank_of_the_process == 0 ) {
            if( sum_different_generations == 0 )
                generation_continue = 0;
            else
                sum_different_generations = 0;
        }
        // send the same generation flag to all processes
        MPI_Bcast(&generation_continue, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // stop Wtime and Profiling
    local_end = MPI_Wtime();
    MPI_Pcontrol(0);
    // calculate local elapsed time
    local_elapsed = local_end - local_start;
    // get elapsed time by each process and find the max elapsed time
    MPI_Reduce(&local_elapsed,&max_elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
    // print max elapsed time
    if( rank_of_the_process == 0 )
        printf("Elapsed time = %f\n",max_elapsed);
    
    // free all structures
    MPI_Type_free(&blocktype_1);
    MPI_Type_free(&blocktype_2);
    free_pointer(&sendcounts);
    free_pointer(&displs);
    free_grid_side_dimensions(&grid_side_dimensions);
    free_grid(&current_generation);
    free_grid(&next_generation);
    free_pointer(&request);
    if( rank_of_the_process == 0 )
        free_grid(&grid);
    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}