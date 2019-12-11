#include "../../header/utilities.h"

void parse_arguments(struct arguments *arguments,int argc, char **argv) {
    int option;
    while((option = getopt(argc, argv,"d:l:i:o")) != -1){
        switch(option){
            case 'd':
                arguments->dimension = atoi(optarg);
                break;
            case 'l':
                arguments->loops = atoi(optarg);
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
    printf("loops = %d\n",arguments.loops);
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

void calculate_neighbor_coords_processes(struct neighbor_processes *neighbor_processes, int *coords) {
    neighbor_processes->bottom_neighbor_coords[0] = coords[0] + 1;
    neighbor_processes->bottom_neighbor_coords[1] = coords[1];
    neighbor_processes->bottom_right_neighbor_coords[0] = coords[0] + 1;
    neighbor_processes->bottom_right_neighbor_coords[1] = coords[1] + 1;
    neighbor_processes->bottom_left_neighbor_coords[0] = coords[0] + 1;
    neighbor_processes->bottom_left_neighbor_coords[1] = coords[1] - 1;
    neighbor_processes->top_neighbor_coords[0] = coords[0] - 1;
    neighbor_processes->top_neighbor_coords[1] = coords[1];
    neighbor_processes->top_right_neighbor_coords[0] = coords[0] - 1;
    neighbor_processes->top_right_neighbor_coords[1] = coords[1] + 1;
    neighbor_processes->top_left_neighbor_coords[0] = coords[0] - 1;
    neighbor_processes->top_left_neighbor_coords[1] = coords[1] - 1;
    neighbor_processes->right_neighbor_coords[0] = coords[0];
    neighbor_processes->right_neighbor_coords[1] = coords[1] + 1;
    neighbor_processes->left_neighbor_coords[0] = coords[0];
    neighbor_processes->left_neighbor_coords[1] = coords[1] - 1;
}

void print_neighbor_ranks(struct neighbor_processes neighbor_processes, int rank_of_the_process) {
    printf("Process rank %d\nNeighbors Ranks: bottom = %d, bottom_right = %d, bottom_left = %d, top = %d, top_right = %d, top_left = %d, right = %d, left = %d \n"
            ,rank_of_the_process
            ,neighbor_processes.bottom_neighbor_rank,neighbor_processes.bottom_right_neighbor_rank
            ,neighbor_processes.bottom_left_neighbor_rank,neighbor_processes.top_neighbor_rank
            ,neighbor_processes.top_right_neighbor_rank,neighbor_processes.top_left_neighbor_rank,
            neighbor_processes.right_neighbor_rank,neighbor_processes.left_neighbor_rank);
}