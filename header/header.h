#ifndef header_h_
#define header_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#define BUFFER_SIZE 256
#define DEFAULT_DIMENSION 840
#define DEFAULT_GENERATIONS 100
#define DEFAULT_INPUTFILE "\0"
#define DEFAULT_OUTPUT false
#define DEFAULT_THREADS 2
#define SUCCESS 0
#define FAILURE -1

#define error_handler(expression, message)                                  \
    do {                                                                    \
        if( (expression) == true ) {                                        \
            fprintf(stderr, "%s:%u: %s: '" #expression "' %s.\n",           \
                    __FILE__, __LINE__, __func__,message);                  \
            exit(FAILURE);                                                  \
        }                                                                   \
    } while (0)

#define my_malloc(type, number_of_elements)                                 \
    ( (type*) malloc( (number_of_elements) * sizeof(type)) )

#define my_printf(x) _Generic((x),                                          \
    int: "%d ",                                                             \
    double: "%f ",                                                          \
    char: "%c ",                                                            \
    char *: "%s ")
#define print(x) printf(#x" = "), printf(my_printf(x), x);
#define printnl(x) do { print(x); printf("\n"); } while(0)

#define swap(x,y)                                                                   \
    do {                                                                            \
        unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1];   \
        memcpy(swap_temp,&y,sizeof(x));                                             \
        memcpy(&y,&x,       sizeof(x));                                             \
        memcpy(&x,swap_temp,sizeof(x));                                             \
    } while(0)

#endif