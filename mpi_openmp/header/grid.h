#ifndef grid_H_
#define grid_H_

#include "./header.h"
#include "./utilities.h"

struct grid_side_dimensions {
    char *top_dimension;
    char *bottom_dimension;
    char *left_dimension;
    char *right_dimension;
    char top_left_corner, top_right_corner;
    char bottom_left_corner, bottom_right_corner;
};

struct grid {
    char **array;
    int dimension;
};

struct grid_side_dimensions *allocate_grid_side_dimensions(int );
char **allocate_2d_array(int );
struct grid *allocate_grid(int);
void initialize_grid(struct grid **);
void initialize_grid_from_inputfile(struct grid **, char *);
void initialize_sendcounts_and_displs_for_scattering_the_grid(int *, int *, int, int , int);
int calculate_subgrid_dimension(int ,int );
void print_grid(struct grid *, int , char *, int );
void print_sendcounts_and_displs(int *, int *, int);
void print_grid_side_dimensions(struct grid_side_dimensions *, int , int);
void print_1d_array(char *, int );
void free_2d_array(char ***);
void free_grid(struct grid **);
void free_grid_side_dimensions(struct grid_side_dimensions **);

static inline char apply_rules(char state, int neighbours) {
    // if current state is a dead cell and has exactly 3 neighbours then the state becomes a live cell
    if( (state == 0) && (neighbours == 3))
        return 1;
    // if current state is a live cell and has fewer than 2 or more than 3 neighbours then the state becomes a dead cell
    else if( (state == 1) && ((neighbours < 2) || (neighbours > 3)) )
        return 0;
    // otherwise, if the current state has 2 or 3 neighbors lives on to the next generation
    else
        return state;
}

static inline void swap_grids(struct grid **current_generation,struct grid **next_generation) {
    struct grid *temp_grid;
    temp_grid = *current_generation;
    *current_generation = *next_generation;
    *next_generation = temp_grid;
}

#endif