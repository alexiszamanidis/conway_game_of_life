#include "../header/grid.h"
#include "../header/grid.h"
#include "../header/utilities.h"

// mpiexec -n 4 ./game_of_life -d 10 -l 2 -i ../../inputfiles/grid_10x10.csv -o

int main( int argc, char **argv ) {
    double local_start, local_end, local_elapsed, max_elapsed;
    int number_of_processes, rank_of_the_process, generation, i, j;
    int neighbours, last, subgrid_dimension, process_grid_dimension, *sendcounts, *displs;
    struct grid *grid = NULL,*current_generation = NULL, *next_generation = NULL;
    struct neighbour_processes neighbour_processes;
    struct grid_side_dimensions *grid_side_dimensions = NULL;
    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimension = DEFAULT_DIMENSION, .generations = DEFAULT_GENERATIONS, .inputfile = DEFAULT_INPUTFILE, .output = DEFAULT_OUTPUT, .threads = DEFAULT_THREADS };
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
    MPI_Request request[16];
    MPI_Datatype columns;

    // allocate and initialize grid
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
//    print_sendcounts_and_displs(sendcounts,displs,grid);
    // scatter a buffer in blocks to all processes in a communicator  
    MPI_Scatterv(&(grid->array[0][0]), sendcounts, displs, blocktype_1, &current_generation->array[0][0], subgrid_dimension*subgrid_dimension, MPI_CHAR, 0, MPI_COMM_WORLD);

    // calculate neighbour coordinates and ranks
    calculate_neighbours(&neighbour_processes,rank_of_the_process,process_grid_dimension);
//    print_neighbour_ranks(neighbour_processes,rank_of_the_process);

    MPI_Type_vector(current_generation->dimension, 1, current_generation->dimension, MPI_CHAR, &columns);
    MPI_Type_commit(&columns);

    // print arguments and global grid if the user gave -o output option for printing/debugging
    if( (rank_of_the_process == 0) && (arguments.output == true) ) {
        print_arguments(arguments);
        print_grid(grid, rank_of_the_process, "global_grid", 0);
    }

    // start Wtime and Profiling
    MPI_Barrier(MPI_COMM_WORLD);
    local_start = MPI_Wtime();
    MPI_Pcontrol(1);

    // basic structure of central iteration:
    for( generation = 0 ; generation < arguments.generations ; generation++ ) {
        
        // receive all neighbours
        MPI_Irecv(grid_side_dimensions->bottom_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.top_neighbour_rank, 0,MPI_COMM_WORLD, &request[8]);
        MPI_Irecv(grid_side_dimensions->top_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.bottom_neighbour_rank, 0,MPI_COMM_WORLD, &request[9]);
        MPI_Irecv(grid_side_dimensions->right_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.left_neighbour_rank, 0,MPI_COMM_WORLD, &request[10]);
        MPI_Irecv(grid_side_dimensions->left_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.right_neighbour_rank, 0,MPI_COMM_WORLD, &request[11]);
        MPI_Irecv(&grid_side_dimensions->bottom_right_corner, 1, MPI_CHAR, neighbour_processes.top_left_neighbour_rank, 0, MPI_COMM_WORLD, &request[12]);
        MPI_Irecv(&grid_side_dimensions->bottom_left_corner, 1, MPI_CHAR, neighbour_processes.top_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[13]);
        MPI_Irecv(&grid_side_dimensions->top_right_corner, 1, MPI_CHAR, neighbour_processes.bottom_left_neighbour_rank, 0, MPI_COMM_WORLD, &request[14]);
        MPI_Irecv(&grid_side_dimensions->top_left_corner, 1, MPI_CHAR, neighbour_processes.bottom_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[15]);

        // send all neighbours
        MPI_Isend(&current_generation->array[0][0], current_generation->dimension, MPI_CHAR, neighbour_processes.top_neighbour_rank, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(&current_generation->array[current_generation->dimension-1][0], current_generation->dimension, MPI_CHAR, neighbour_processes.bottom_neighbour_rank, 0, MPI_COMM_WORLD, &request[1]);
        MPI_Isend(&current_generation->array[0][0], 1, columns, neighbour_processes.left_neighbour_rank, 0,MPI_COMM_WORLD, &request[2]);
        MPI_Isend(&current_generation->array[0][current_generation->dimension-1], 1, columns, neighbour_processes.right_neighbour_rank, 0, MPI_COMM_WORLD, &request[3]);
        MPI_Isend(&current_generation->array[0][0], 1, MPI_CHAR, neighbour_processes.top_left_neighbour_rank, 0,MPI_COMM_WORLD, &request[4]);
        MPI_Isend(&current_generation->array[0][current_generation->dimension-1], 1, MPI_CHAR, neighbour_processes.top_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[5]);
        MPI_Isend(&current_generation->array[current_generation->dimension-1][0], 1, MPI_CHAR, neighbour_processes.bottom_left_neighbour_rank, 0,MPI_COMM_WORLD, &request[6]);
        MPI_Isend(&current_generation->array[current_generation->dimension-1][current_generation->dimension-1],1, MPI_CHAR, neighbour_processes.bottom_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[7]);
        
        // calculate intermidiate elements
        for( i = 1 ; i < current_generation->dimension-1 ; i++ ) {
            for( j = 1 ; j < current_generation->dimension-1 ; j++ ) {
                // sum all alive neighbours
                neighbours = (current_generation->array[i-1][j-1]-'0')+(current_generation->array[i-1][j]-'0')+(current_generation->array[i-1][j+1]-'0')
                            +(current_generation->array[i][j-1]-'0')+(current_generation->array[i][j+1]-'0')
                            +(current_generation->array[i+1][j-1]-'0')+(current_generation->array[i+1][j]-'0')+(current_generation->array[i+1][j+1]-'0');
                // apply the rules and create next generation grid
                next_generation->array[i][j] = apply_rules(current_generation->array[i][j],neighbours);
            }
        }

        // wait for all given communications to complete
        MPI_Waitall(16, request, status);   
    //    print_grid_side_dimensions(grid_side_dimensions,current_generation->dimension,rank_of_the_process);

        // calculate outline elements
        for( i = 1 ; i < current_generation->dimension-1 ; i++ ) {
            // top dimension: sum all alive neighbours
            neighbours = (current_generation->array[0][i-1]-'0')+(current_generation->array[0][i+1]-'0')
                        +(current_generation->array[1][i-1]-'0')+(current_generation->array[1][i]-'0')+(current_generation->array[1][i+1]-'0')
                        +(grid_side_dimensions->top_dimension[i-1]-'0')+(grid_side_dimensions->top_dimension[i]-'0')+(grid_side_dimensions->top_dimension[i+1]-'0');
            // apply the rules and create next generation grid
            next_generation->array[0][i] = apply_rules(current_generation->array[0][i],neighbours);

            // bottom dimension: sum all alive neighbours
            neighbours = (current_generation->array[last][i-1]-'0')+(current_generation->array[last][i+1]-'0')
                        +(current_generation->array[last-1][i-1]-'0')+(current_generation->array[last-1][i]-'0')+(current_generation->array[last-1][i+1]-'0')
                        +(grid_side_dimensions->bottom_dimension[i-1]-'0')+(grid_side_dimensions->bottom_dimension[i]-'0')+(grid_side_dimensions->bottom_dimension[i+1]-'0');
            // apply the rules and create next generation grid
            next_generation->array[last][i] = apply_rules(current_generation->array[last][i],neighbours);

            // left dimension: sum all alive neighbours
            neighbours = (current_generation->array[i-1][0]-'0')+(current_generation->array[i+1][0]-'0')
                        +(current_generation->array[i-1][1]-'0')+(current_generation->array[i][1]-'0')+(current_generation->array[i+1][1]-'0')
                        +(grid_side_dimensions->left_dimension[i-1]-'0')+(grid_side_dimensions->left_dimension[i]-'0')+(grid_side_dimensions->left_dimension[i+1]-'0');
            // apply the rules and create next generation grid
            next_generation->array[i][0] = apply_rules(current_generation->array[i][0],neighbours);

            // right dimension: sum all alive neighbours
            neighbours = (current_generation->array[i-1][last]-'0')+(current_generation->array[i+1][last]-'0')
                        +(current_generation->array[i-1][last-1]-'0')+(current_generation->array[i][last-1]-'0')+(current_generation->array[i+1][last-1]-'0')
                        +(grid_side_dimensions->right_dimension[i-1]-'0')+(grid_side_dimensions->right_dimension[i]-'0')+(grid_side_dimensions->right_dimension[i+1]-'0');
            // apply the rules and create next generation grid
            next_generation->array[i][last] = apply_rules(current_generation->array[i][last],neighbours);
        }
        // top left corner cell: sum all alive neighbours
        neighbours = (current_generation->array[0][1]-'0')+(current_generation->array[1][0]-'0')+(current_generation->array[1][1]-'0')
                    +(grid_side_dimensions->top_dimension[0]-'0')+(grid_side_dimensions->top_dimension[1]-'0')
                    +(grid_side_dimensions->left_dimension[0]-'0')+(grid_side_dimensions->left_dimension[1]-'0')
                    +(grid_side_dimensions->top_left_corner-'0');
        // apply the rules and create next generation grid
        next_generation->array[0][0] = apply_rules(current_generation->array[0][0],neighbours);

        // top right corner cell: sum all alive neighbours
        neighbours = (current_generation->array[0][last-1]-'0')+(current_generation->array[1][last]-'0')+(current_generation->array[1][last-1]-'0')
                    +(grid_side_dimensions->top_dimension[last]-'0')+(grid_side_dimensions->top_dimension[last-1]-'0')
                    +(grid_side_dimensions->right_dimension[0]-'0')+(grid_side_dimensions->right_dimension[1]-'0')
                    +(grid_side_dimensions->top_right_corner-'0');
        // apply the rules and create next generation grid
        next_generation->array[0][last] = apply_rules(current_generation->array[0][last],neighbours);

        // bottom left corner cell: sum all alive neighbours
        neighbours = (current_generation->array[last-1][0]-'0')+(current_generation->array[last-1][1]-'0')+(current_generation->array[last][1]-'0')
                    +(grid_side_dimensions->bottom_dimension[0]-'0')+(grid_side_dimensions->bottom_dimension[1]-'0')
                    +(grid_side_dimensions->left_dimension[last]-'0')+(grid_side_dimensions->left_dimension[last-1]-'0')
                    +(grid_side_dimensions->bottom_left_corner-'0');
        // apply the rules and create next generation grid
        
        next_generation->array[last][0] = apply_rules(current_generation->array[last][0],neighbours);
        // bottom right corner cell: sum all alive neighbours
        neighbours = (current_generation->array[last-1][last]-'0')+(current_generation->array[last-1][last-1]-'0')+(current_generation->array[last][last-1]-'0')
                    +(grid_side_dimensions->bottom_dimension[last]-'0')+(grid_side_dimensions->bottom_dimension[last-1]-'0')
                    +(grid_side_dimensions->right_dimension[last]-'0')+(grid_side_dimensions->right_dimension[last-1]-'0')
                    +(grid_side_dimensions->bottom_right_corner-'0');
        // apply the rules and create next generation grid
        next_generation->array[last][last] = apply_rules(current_generation->array[last][last],neighbours);

        // gather global global grid, print current generation and next generation grid for each process if the user gave -o output option for printing/debugging
        if (arguments.output == true) {
            MPI_Gatherv(&(next_generation->array[0][0]), next_generation->dimension * next_generation->dimension, MPI_CHAR, &(grid->array[0][0]), sendcounts, displs, blocktype_1, 0,MPI_COMM_WORLD);
            print_grid(current_generation, rank_of_the_process, "current_generation", generation+1);
            print_grid(next_generation, rank_of_the_process, "next_generation", generation+1);
            // if the process's rank is 0, then print current generation to stdout and global grid
            if( rank_of_the_process == 0 ) {
                printf("Generation: %d\n", generation+1);
                print_grid(grid, rank_of_the_process, "global_grid", generation+1);
            }
        }
        // swap generations
        swap_grids(&current_generation,&next_generation);
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
    free(sendcounts);
    free(displs);
    free_grid_side_dimensions(&grid_side_dimensions);
    free_grid(&current_generation);
    free_grid(&next_generation);
    if( rank_of_the_process == 0 )
        free_grid(&grid);
    // finalize the MPI environment
    MPI_Finalize();

    return SUCCESS;
}