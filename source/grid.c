#include "../header/grid.h"

struct grid *allocate_grid(int dimention) {
    struct grid *grid = (struct grid *)malloc(sizeof(struct grid));
    if( grid == NULL ) {
        printf("allocate_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    grid->array = (char **)malloc(dimention*sizeof(char *));
    if( grid->array == NULL) {
        printf("allocate_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    for( int i = 0 ; i < dimention ; i++) {
        grid->array[i] = (char *)malloc(dimention*sizeof(char));
        if( grid->array[i] == NULL ) {
            printf("allocate_grid: %s\n",strerror(errno));
            exit(FAILURE);
        }
    }

    grid->dimention = dimention;

    return grid;
}

void initialize_grid(struct grid **grid) {
    int number;
    for( int i = 0 ; i < (*grid)->dimention ; i++ ) {
        for( int j = 0 ; j < (*grid)->dimention ; j++ ) {
            number = rand() % 100 + 1;
            if (number < 30)
                (*grid)->array[i][j] = '*';
            else
                (*grid)->array[i][j] = '.';
        }
    }
}

void print_grid(struct grid *grid, char *filename) {
    FILE *file_pointer = fopen(filename, "w+");
    if ( file_pointer == NULL ) {
        printf("print_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    for( int i = 0 ; i < grid->dimention ; i++ ) {
        for( int j = 0 ; j < grid->dimention ; j++ )
            fprintf(file_pointer, "%c",grid->array[i][j]);
        fprintf(file_pointer, "\n");
    }
    fclose(file_pointer);
}

void free_grid(struct grid **grid) {
    for( int i = 0 ; i < (*grid)->dimention ; i++ )
        free((*grid)->array[i]);
    free((*grid)->array);
    free((*grid));
}