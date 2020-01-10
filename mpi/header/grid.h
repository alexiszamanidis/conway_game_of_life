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
char apply_rules(char , int );
void swap_grids(struct grid **,struct grid **);
void print_grid(struct grid *, int , char *, int );
void print_sendcounts_and_displs(int *, int *, int);
void print_grid_side_dimensions(struct grid_side_dimensions *, int , int);
void print_1d_array(char *, int );
void free_2d_array(char ***);
void free_grid(struct grid **);
void free_grid_side_dimensions(struct grid_side_dimensions **);

#endif