#ifndef utilities_H_
#define utilities_H_

#include "./header.h"

struct arguments {
    int dimension;
    int loops;
    char inputfile[BUFFER_SIZE];
    bool output;
};

void parse_arguments(struct arguments *,int , char **);
void print_arguments(struct arguments );

#endif