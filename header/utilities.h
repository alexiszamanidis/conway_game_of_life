#ifndef utilities_H_
#define utilities_H_

#include "./header.h"

struct arguments {
    int dimension;
    int loops;
    char inputfile[BUFFER_SIZE];
    bool output;
};

struct neighbor_processes {
    // neighbor ranks
    int bottom_neighbor_rank;
    int bottom_right_neighbor_rank;
    int bottom_left_neighbor_rank;
    int top_neighbor_rank;
    int top_right_neighbor_rank;
    int top_left_neighbor_rank;
    int right_neighbor_rank;
    int left_neighbor_rank;
    // neighbor coords
    int bottom_neighbor_coords[2];
    int bottom_right_neighbor_coords[2];
    int bottom_left_neighbor_coords[2];
    int top_neighbor_coords[2];
    int top_right_neighbor_coords[2];
    int top_left_neighbor_coords[2];
    int right_neighbor_coords[2];
    int left_neighbor_coords[2];
};

void parse_arguments(struct arguments *,int , char **);
void print_arguments(struct arguments );
int is_perfect_square(int );
void calculate_neighbor_coords_processes(struct neighbor_processes *, int *);
void print_neighbor_ranks(struct neighbor_processes , int );

#endif