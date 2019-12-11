#include "../../header/grid.h"

struct grid *allocate_grid(int dimension, int number_of_processes) {
    struct grid *grid = (struct grid *)malloc(sizeof(struct grid));
    if( grid == NULL ) {
        printf("allocate_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    grid->array = (char **)malloc(dimension*sizeof(char *));
    if( grid->array == NULL) {
        printf("allocate_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    for( int i = 0 ; i < dimension ; i++) {
        grid->array[i] = (char *)malloc(dimension*sizeof(char));
        if( grid->array[i] == NULL ) {
            printf("allocate_grid: %s\n",strerror(errno));
            exit(FAILURE);
        }
    }

    grid->dimension = dimension;
    calculate_subgrid_dimension(&grid,number_of_processes);

    return grid;
}

void initialize_grid(struct grid **grid) {
    char array_content[] = { '*', '.' };
    for( int i = 0 ; i < (*grid)->dimension ; i++ )
        for( int j = 0 ; j < (*grid)->dimension ; j++ )
            (*grid)->array[i][j] = array_content[ rand() % 2 ];
}

void initialize_grid_from_inputfile(struct grid **grid, char *inputfile) {
    FILE *file_pointer = fopen(inputfile, "r");
    if ( file_pointer == NULL ) {
        printf("initialize_grid_from_inputfile: %s\n",strerror(errno));
        exit(FAILURE);
    }
    char *line=NULL;
    size_t length=0;
    for( int i = 0 ; i < (*grid)->dimension ; i++ ) {
        getline(&line, &length, file_pointer);
        line[strlen(line)-1] = '\0';
        for( int j = 0 ; j < (*grid)->dimension ; j++ )
            (*grid)->array[i][j] = line[j];
    }
    free(line);
    fclose(file_pointer);
}

void print_grid(struct grid *grid, char *filename) {
    FILE *file_pointer = fopen(filename, "w+");
    if ( file_pointer == NULL ) {
        printf("print_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }
    fprintf(file_pointer, "Dimension = %d, Subgrid dimension = %d \n",grid->dimension, grid->subgrid_dimension);
    for( int i = 0 ; i < grid->dimension ; i++ ) {
        for( int j = 0 ; j < grid->dimension ; j++ )
            fprintf(file_pointer, "%c",grid->array[i][j]);
        fprintf(file_pointer, "\n");
    }
    fclose(file_pointer);
}

void calculate_subgrid_dimension(struct grid **grid,int number_of_processes) {
    int root = is_perfect_square(number_of_processes);
    
    if( root == -1 ) {
        printf("calculate_subgrid_dimension: number of processes should be perfect square\n");
        exit(FAILURE);
    }
    else if( ((*grid)->dimension % root) != 0 ) {
        printf("calculate_subgrid_dimension: number of processes can not divide perfectly the dimension\n");
        exit(FAILURE);
    }
    else
        (*grid)->subgrid_dimension = ((*grid)->dimension / root);
}

void free_grid(struct grid **grid) {
    for( int i = 0 ; i < (*grid)->dimension ; i++ )
        free((*grid)->array[i]);
    free((*grid)->array);
    free((*grid));
}