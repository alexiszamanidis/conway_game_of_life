#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

// mpiexec -n 4 ./game_of_life -d 20 -i ../inputfiles/grid_20x20.csv -o

int main( int argc, char **argv ) {
    int number_of_processes, rank_of_the_process, reorder = 1;
    double local_start, local_end, local_elapsed, max_elapsed;
    char **local_grid, **next_local_grid;
    int periods[2]= {1,1}, dim[2], coords[2];
    struct neighbor_processes neighbor_processes;
    struct grid_side_dimensions *grid_side_dimensions = NULL;
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
    if( rank_of_the_process == 0 ) {
        if( strcmp(arguments.inputfile,"") == 0 )
            initialize_grid(&grid);
        else
            initialize_grid_from_inputfile(&grid,arguments.inputfile);
    }

    // allocate grid side dimensions,local grid, next local grid and initialize dim for mpi cart
    grid_side_dimensions = allocate_grid_side_dimensions(grid->subgrid_dimension);
    local_grid = allocate_2d_array(grid->subgrid_dimension);
    next_local_grid = allocate_2d_array(grid->subgrid_dimension);
    dim[0] = grid->process_grid_dimension;
    dim[1] = grid->process_grid_dimension;

    // print array if the user gave -o output option
    if( (rank_of_the_process == 0) && (arguments.output == true) ) {
    //    print_arguments(arguments);
        print_grid(grid,"output.csv");
    }

    MPI_Datatype block_1, block_2;
    MPI_Type_vector(grid->subgrid_dimension, grid->subgrid_dimension, grid->dimension, MPI_CHAR, &block_2);
    MPI_Type_create_resized(block_2, 0, sizeof(char), &block_1);
    MPI_Type_commit(&block_1);

    int sendcounts[(int)pow(grid->process_grid_dimension,2)],displs[(int)pow(grid->process_grid_dimension,2)];

    // initialize sendcounts, displacements and scatter the grid
    initialize_sendcounts_and_displs_for_scattering_the_grid(sendcounts,displs,grid);
//    print_sendcounts_and_displs(sendcounts,displs,grid);
    MPI_Scatterv(&(grid->array[0][0]), sendcounts, displs, block_1, &local_grid[0][0], grid->subgrid_dimension*grid->subgrid_dimension, MPI_CHAR, 0, MPI_COMM_WORLD);

    print_2d_array(local_grid,grid->subgrid_dimension,rank_of_the_process);
    
    MPI_Comm comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, periods, reorder, &comm);

    // determines process coords in cartesian topology given rank in group
    MPI_Cart_coords(comm, rank_of_the_process, 2, coords);
//    printf("rank = %d, coords[0] = %d, coords[1] = %d\n",rank_of_the_process,coords[0],coords[1]);

    calculate_neighbor_coords_processes(&neighbor_processes,coords);

    MPI_Cart_rank(comm, neighbor_processes.bottom_neighbor_coords, &neighbor_processes.bottom_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.bottom_right_neighbor_coords, &neighbor_processes.bottom_right_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.bottom_left_neighbor_coords, &neighbor_processes.bottom_left_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.top_neighbor_coords, &neighbor_processes.top_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.top_right_neighbor_coords, &neighbor_processes.top_right_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.top_left_neighbor_coords, &neighbor_processes.top_left_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.right_neighbor_coords, &neighbor_processes.right_neighbor_rank);
    MPI_Cart_rank(comm, neighbor_processes.left_neighbor_coords, &neighbor_processes.left_neighbor_rank);

//    print_neighbor_ranks(neighbor_processes,rank_of_the_process);

    MPI_Status status[16];
    MPI_Request request[16];
    MPI_Datatype columns;
    MPI_Type_vector(grid->subgrid_dimension, 1, grid->subgrid_dimension, MPI_CHAR, &columns);
    MPI_Type_commit(&columns);

    for(  int i = 0 ; i < arguments.loops ; i++) {
        if( (rank_of_the_process == 0) && (arguments.output == true) ) {
            printf("generation: %d\n", i+1);
        }
        // send all neighbors
        MPI_Isend(&local_grid[0][0], grid->subgrid_dimension, MPI_CHAR, neighbor_processes.top_neighbor_rank, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(&local_grid[grid->subgrid_dimension-1][0], grid->subgrid_dimension, MPI_CHAR, neighbor_processes.bottom_neighbor_rank, 0, MPI_COMM_WORLD, &request[1]);
        MPI_Isend(&local_grid[0][0], 1, columns, neighbor_processes.left_neighbor_rank, 0,MPI_COMM_WORLD, &request[2]);
        MPI_Isend(&local_grid[0][grid->subgrid_dimension-1], 1, columns, neighbor_processes.right_neighbor_rank, 0, MPI_COMM_WORLD, &request[3]);
        MPI_Isend(&local_grid[0][0], 1, MPI_CHAR, neighbor_processes.top_left_neighbor_rank, 0,MPI_COMM_WORLD, &request[4]);
        MPI_Isend(&local_grid[0][grid->subgrid_dimension-1], 1, MPI_CHAR, neighbor_processes.top_right_neighbor_rank, 0, MPI_COMM_WORLD, &request[5]);
        MPI_Isend(&local_grid[grid->subgrid_dimension-1][0], 1, MPI_CHAR, neighbor_processes.bottom_left_neighbor_rank, 0,MPI_COMM_WORLD, &request[6]);
        MPI_Isend(&local_grid[grid->subgrid_dimension-1][grid->subgrid_dimension-1],1, MPI_CHAR, neighbor_processes.bottom_right_neighbor_rank, 0, MPI_COMM_WORLD, &request[7]);

        // receive all neighbors
        MPI_Irecv(grid_side_dimensions->top_dimension, grid->subgrid_dimension, MPI_CHAR, neighbor_processes.top_neighbor_rank, 0,MPI_COMM_WORLD, &request[8]);
        MPI_Irecv(grid_side_dimensions->bottom_dimension, grid->subgrid_dimension, MPI_CHAR, neighbor_processes.bottom_neighbor_rank, 0,MPI_COMM_WORLD, &request[9]);
        MPI_Irecv(grid_side_dimensions->left_dimension, grid->subgrid_dimension, MPI_CHAR, neighbor_processes.left_neighbor_rank, 0,MPI_COMM_WORLD, &request[10]);
        MPI_Irecv(grid_side_dimensions->right_dimension, grid->subgrid_dimension, MPI_CHAR, neighbor_processes.right_neighbor_rank, 0,MPI_COMM_WORLD, &request[11]);
        MPI_Irecv(&grid_side_dimensions->top_left, 1, MPI_CHAR, neighbor_processes.top_left_neighbor_rank, 0, MPI_COMM_WORLD, &request[12]);
        MPI_Irecv(&grid_side_dimensions->top_right, 1, MPI_CHAR, neighbor_processes.top_right_neighbor_rank, 0, MPI_COMM_WORLD, &request[13]);
        MPI_Irecv(&grid_side_dimensions->bottom_left, 1, MPI_CHAR, neighbor_processes.bottom_left_neighbor_rank, 0, MPI_COMM_WORLD, &request[14]);
        MPI_Irecv(&grid_side_dimensions->bootom_right, 1, MPI_CHAR, neighbor_processes.bottom_right_neighbor_rank, 0, MPI_COMM_WORLD, &request[15]);
        
        if( rank_of_the_process == 0) {
            // calculate intermidiate elements
            for( int i = 1 ; i < grid->subgrid_dimension-1 ; i++ ) {
                for( int j = 1 ; j < grid->subgrid_dimension-1 ; j++ ) {
                    int neighbors = 0;
                    // calculate neighbors
                    for( int k = -1 ; k < 2 ; k++ ) {
                        for( int l = -1 ; l < 2 ; l++ ) {
                            if( (local_grid[i+k][j+l] == '*') && ((k!=0)||(l!=0)))
                                neighbors++;
                        }
                    }
                }
            }
        }

        // wait for all given communications to complete
        MPI_Waitall(16, request, status);
        
    //    print_grid_side_dimensions(grid_side_dimensions,grid->subgrid_dimension,rank_of_the_process);
    }

    // stop Wtime and Profiling
    local_end = MPI_Wtime();
    MPI_Pcontrol(0);
    // calculate local elapsed time
    local_elapsed = local_end - local_start;
    // get elapsed time by each process and find the max elapsed time
    MPI_Reduce(&local_elapsed,&max_elapsed,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);

    if( rank_of_the_process == 0 )
        printf("Elapsed time = %f\n",max_elapsed);
    
    free_grid_side_dimensions(&grid_side_dimensions);
    free_2d_array(&local_grid);
    free_2d_array(&next_local_grid);
    if( rank_of_the_process == 0 )
        free_grid(&grid);
    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}