#ifndef grid_H_
#define grid_H_

#include "./header.h"

struct grid {
    char **array;
    int dimention;
};

struct grid *allocate_grid(int );
void initialize_grid(struct grid **);
void initialize_grid_from_inputfile(struct grid **, char *);
void print_grid(struct grid *, char *);
void free_grid(struct grid **);

#endif