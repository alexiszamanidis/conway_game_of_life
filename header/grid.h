#ifndef grid_H_
#define grid_H_

#include "./header.h"
#include "./utilities.h"

struct grid_side_dimensions {
    char *top_dimension;
    char *bottom_dimension;
    char *left_dimension;
    char *right_dimension;
    char top_left, top_right;
    char bottom_left, bootom_right;
};

struct grid {
    char **array;
    int dimension;
    int subgrid_dimension;
    int process_grid_dimension;
};

struct grid_side_dimensions *allocate_grid_side_dimensions(int );
char **allocate_2d_array(int );
struct grid *allocate_grid(int , int);
void initialize_grid(struct grid **);
void initialize_grid_from_inputfile(struct grid **, char *);
void print_2d_array(char **, int, int);
void print_grid(struct grid *, char *);
void calculate_subgrid_dimension(struct grid **,int );
void free_2d_array(char ***);
void free_grid(struct grid **);
void free_grid_side_dimensions(struct grid_side_dimensions **);
void initialize_sendcounts_and_displs_for_scattering_the_grid(int *, int *, struct grid * );
void print_sendcounts_and_displs(int *, int *, struct grid *);

#endif