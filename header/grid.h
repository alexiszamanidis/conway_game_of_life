#ifndef grid_H_
#define grid_H_

#include "./header.h"
#include "./utilities.h"

struct grid {
    char **array;
    int dimension;
    int subgrid_dimension;
};

struct grid *allocate_grid(int , int);
void initialize_grid(struct grid **);
void initialize_grid_from_inputfile(struct grid **, char *);
void print_grid(struct grid *, char *);
void calculate_subgrid_dimension(struct grid **,int );
void free_grid(struct grid **);

#endif