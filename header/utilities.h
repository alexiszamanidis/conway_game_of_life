#ifndef utilities_h_
#define utilities_h_

#include "./header.h"

struct arguments {
    int dimension;
    int generations;
    char inputfile[BUFFER_SIZE];
    int threads;
};

struct neighbour_processes {
    // neighbour ranks
    int bottom_neighbour_rank;
    int bottom_right_neighbour_rank;
    int bottom_left_neighbour_rank;
    int top_neighbour_rank;
    int top_right_neighbour_rank;
    int top_left_neighbour_rank;
    int right_neighbour_rank;
    int left_neighbour_rank;
    // neighbour coordinates
    int bottom_neighbour_coordinates[2];
    int bottom_right_neighbour_coordinates[2];
    int bottom_left_neighbour_coordinates[2];
    int top_neighbour_coordinates[2];
    int top_right_neighbour_coordinates[2];
    int top_left_neighbour_coordinates[2];
    int right_neighbour_coordinates[2];
    int left_neighbour_coordinates[2];
};

void parse_arguments(struct arguments *,int , char **);
void print_arguments(struct arguments );
int is_perfect_square(int );
void calculate_neighbours(struct neighbour_processes *, int, int);
void print_neighbour_ranks(struct neighbour_processes , int );

#endif