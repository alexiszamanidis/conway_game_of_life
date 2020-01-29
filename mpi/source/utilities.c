#include "../header/utilities.h"

void parse_arguments(struct arguments *arguments,int argc, char **argv) {
    int option;
    while((option = getopt(argc, argv,"d:g:i:o")) != -1){
        switch(option){
            case 'd':
                arguments->dimension = atoi(optarg);
                break;
            case 'g':
                arguments->generations = atoi(optarg);
                break;
            case 'i':
                strcpy(arguments->inputfile,optarg);
                break;
            case 'o':
                arguments->output = true;
                break;
            case '?':
                printf("parse_arguments: unknown option '%c'\n", optopt);
                exit(FAILURE);
        }
    }
}

void print_arguments(struct arguments arguments) {
    printf("dimension = %d\n",arguments.dimension);
    printf("generations = %d\n",arguments.generations);
    printf("inputfile = %s\n",arguments.inputfile);
    printf(arguments.output ? "output = true\n":"output = false\n");
}

int is_perfect_square(int number) {
    double root = sqrt((double)number);

    if( (root - floor(root)) == 0)
        return root;
    else
        return -1;
}

void calculate_neighbours(struct neighbour_processes *neighbour_processes, int rank_of_the_process, int process_grid_dimension) {
    int periods[2]= {1,1}, dim[2] = { process_grid_dimension, process_grid_dimension}, coordinates[2], reorder = 1, number_of_dimensions = 2;
    MPI_Comm comm;
    // make a new communicator to which topology information has been attached 
    MPI_Cart_create(MPI_COMM_WORLD, number_of_dimensions, dim, periods, reorder, &comm);
    // determines process coordinates in cartesian topology given rank in group
    MPI_Cart_coords(comm, rank_of_the_process, 2, coordinates);
    // calculate neighbour coordinates
    neighbour_processes->bottom_neighbour_coordinates[0] = coordinates[0] + 1;
    neighbour_processes->bottom_neighbour_coordinates[1] = coordinates[1];
    neighbour_processes->bottom_right_neighbour_coordinates[0] = coordinates[0] + 1;
    neighbour_processes->bottom_right_neighbour_coordinates[1] = coordinates[1] + 1;
    neighbour_processes->bottom_left_neighbour_coordinates[0] = coordinates[0] + 1;
    neighbour_processes->bottom_left_neighbour_coordinates[1] = coordinates[1] - 1;
    neighbour_processes->top_neighbour_coordinates[0] = coordinates[0] - 1;
    neighbour_processes->top_neighbour_coordinates[1] = coordinates[1];
    neighbour_processes->top_right_neighbour_coordinates[0] = coordinates[0] - 1;
    neighbour_processes->top_right_neighbour_coordinates[1] = coordinates[1] + 1;
    neighbour_processes->top_left_neighbour_coordinates[0] = coordinates[0] - 1;
    neighbour_processes->top_left_neighbour_coordinates[1] = coordinates[1] - 1;
    neighbour_processes->right_neighbour_coordinates[0] = coordinates[0];
    neighbour_processes->right_neighbour_coordinates[1] = coordinates[1] + 1;
    neighbour_processes->left_neighbour_coordinates[0] = coordinates[0];
    neighbour_processes->left_neighbour_coordinates[1] = coordinates[1] - 1;
    // calculate neighbour ranks
    MPI_Cart_rank(comm, neighbour_processes->bottom_neighbour_coordinates, &neighbour_processes->bottom_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->bottom_right_neighbour_coordinates, &neighbour_processes->bottom_right_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->bottom_left_neighbour_coordinates, &neighbour_processes->bottom_left_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->top_neighbour_coordinates, &neighbour_processes->top_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->top_right_neighbour_coordinates, &neighbour_processes->top_right_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->top_left_neighbour_coordinates, &neighbour_processes->top_left_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->right_neighbour_coordinates, &neighbour_processes->right_neighbour_rank);
    MPI_Cart_rank(comm, neighbour_processes->left_neighbour_coordinates, &neighbour_processes->left_neighbour_rank);
}

void print_neighbour_ranks(struct neighbour_processes neighbour_processes, int rank_of_the_process) {
    printf("Process rank %d\nneighbours Ranks: bottom = %d, bottom_right = %d, bottom_left = %d, top = %d, top_right = %d, top_left = %d, right = %d, left = %d \n"
            ,rank_of_the_process
            ,neighbour_processes.bottom_neighbour_rank,neighbour_processes.bottom_right_neighbour_rank
            ,neighbour_processes.bottom_left_neighbour_rank,neighbour_processes.top_neighbour_rank
            ,neighbour_processes.top_right_neighbour_rank,neighbour_processes.top_left_neighbour_rank,
            neighbour_processes.right_neighbour_rank,neighbour_processes.left_neighbour_rank);
}

int *allocate_1d_array(int number_of_columns) {
    int *array = (int *)malloc(sizeof(int));
    if( array == NULL ){
        printf("allocate_1d_array: %s\n",strerror(errno));
        exit(FAILURE);
    }
    return array;
}