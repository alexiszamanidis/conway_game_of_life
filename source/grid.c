#include "../header/grid.h"

struct grid_side_dimensions *allocate_grid_side_dimensions(int dimension) {
    struct grid_side_dimensions *grid_side_dimensions = my_malloc(struct grid_side_dimensions,1);
    grid_side_dimensions->top_dimension = my_malloc(char,dimension);
    error_handler(grid_side_dimensions->top_dimension == NULL,"malloc failed");
    grid_side_dimensions->bottom_dimension = my_malloc(char,dimension);
    error_handler(grid_side_dimensions->bottom_dimension == NULL,"malloc failed");
    grid_side_dimensions->left_dimension = my_malloc(char,dimension);
    error_handler(grid_side_dimensions->left_dimension == NULL,"malloc failed");
    grid_side_dimensions->right_dimension = my_malloc(char,dimension);
    error_handler(grid_side_dimensions->right_dimension == NULL,"malloc failed");
    return grid_side_dimensions;
}

char **allocate_2d_array(int dimension) {
    // allocate the rows
    char **array = my_malloc(char *,dimension);
    error_handler(array == NULL,"malloc failed");
    
    // allocate all array elements => contiguous allocation
    char *allocate_all_array_elements = my_malloc(char,dimension*dimension);
    error_handler(allocate_all_array_elements == NULL,"malloc failed");

    // fix array rows
    for (int i = 0; i < dimension; i++)
        array[i] = &(allocate_all_array_elements[i * dimension]);

    return array;
}

struct grid *allocate_grid(int dimension) {
    struct grid *grid = my_malloc(struct grid,1);
    error_handler(grid == NULL,"malloc failed");

    grid->array = allocate_2d_array(dimension);
    grid->dimension = dimension;

    return grid;
}

void initialize_grid(struct grid **grid) {
    int array_content[] = { 1, 0 };
    for( int i = 0 ; i < (*grid)->dimension ; i++ )
        for( int j = 0 ; j < (*grid)->dimension ; j++ )
            (*grid)->array[i][j] = array_content[ rand() % 2 ];
}

void initialize_grid_from_inputfile(struct grid **grid, char *inputfile) {
    FILE *file_pointer = fopen(inputfile, "r");
    if( file_pointer == NULL ) {
        printf("initialize_grid_from_inputfile: %s\n",strerror(errno));
        exit(FAILURE);
    }
    char *line=NULL;
    size_t length=0;
    for( int i = 0 ; i < (*grid)->dimension ; i++ ) {
        getline(&line, &length, file_pointer);
        for( int j = 0 ; j < (*grid)->dimension ; j++ )
            if( line[j] != '\0') {
                if( line[j] == '1' )
                    (*grid)->array[i][j] = 1;
                else if( (line[j]) == '0' )
                    (*grid)->array[i][j] = 0;
                else {
                    printf("initialize_grid_from_inputfile: wrong character in inputfile '%s'\n",inputfile);
                    exit(FAILURE);
                }
            }
    }
    free(line);
    fclose(file_pointer);
}

void initialize_sendcounts_and_displs_for_scattering_the_grid(int *sendcounts, int *displs, int dimension,int subgrid_dimension, int process_grid_dimension) {
    for (int i = 0; i < process_grid_dimension; i++)
        for (int j = 0; j < process_grid_dimension; j++) {
            int index = i * process_grid_dimension + j;
            sendcounts[index] = 1;
            displs[index] = (i*dimension*subgrid_dimension) + (j*subgrid_dimension);
        }
}

int calculate_subgrid_dimension(int dimension, int number_of_processes) {
    int root = is_perfect_square(number_of_processes);
    
    if( root == -1 ) {
        printf("calculate_subgrid_dimension: number of processes should be perfect square\n");
        exit(FAILURE);
    }
    else if( (dimension % root) != 0 ) {
        printf("calculate_subgrid_dimension: number of processes can not divide perfectly the dimension\n");
        exit(FAILURE);
    }
    else
        return (dimension / root);
}

void print_grid(struct grid *grid, int rank, char *grid_name, int generation) {
    char filename[200];
    snprintf(filename, 200, "%s_r%d_g%d.csv",grid_name,rank,generation);
    FILE *file_pointer = fopen(filename, "w+");
    if( file_pointer == NULL ) {
        printf("print_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }
//    fprintf(file_pointer, "Dimension = %d, Subgrid dimension = %d, Process grid dimension = %d \n",grid->dimension, grid->subgrid_dimension, grid->process_grid_dimension);
    for( int i = 0 ; i < grid->dimension ; i++ ) {
        for( int j = 0 ; j < grid->dimension ; j++ )
            fprintf(file_pointer, "%d",grid->array[i][j]);
        fprintf(file_pointer, "\n");
    }
    fclose(file_pointer);
}

void print_sendcounts_and_displs(int *sendcounts, int *displs, int process_grid_dimension) {
    for (int i = 0; i < process_grid_dimension; i++)
        for (int j = 0; j < process_grid_dimension; j++) {
            int index = i * process_grid_dimension + j;
            printf("%d ",sendcounts[index]);
        }
    printf("\n");
    for (int i = 0; i < process_grid_dimension; i++)
        for (int j = 0; j < process_grid_dimension; j++) {
            int index = i * process_grid_dimension + j;
            printf("%d ",displs[index]);
        }
    printf("\n");
}

void print_grid_side_dimensions(struct grid_side_dimensions *grid_side_dimensions, int dimension, int rank_of_the_process) {
    printf("rank = %d\nT-side:",rank_of_the_process);
    for( int i = 0 ; i < dimension ; i++)
        printf("%c ",grid_side_dimensions->top_dimension[i]);
    printf("\nB-side:");
    for( int i = 0 ; i < dimension ; i++)
        printf("%c ",grid_side_dimensions->bottom_dimension[i]);
    printf("\nL-side:");
    for( int i = 0 ; i < dimension ; i++)
        printf("%c ",grid_side_dimensions->left_dimension[i]);
    printf("\nR-side:");
    for( int i = 0 ; i < dimension ; i++)
        printf("%c ",grid_side_dimensions->right_dimension[i]);
    printf("\nTL:%c TR:%c BL:%c BR:%c\n",grid_side_dimensions->top_left_corner,grid_side_dimensions->top_right_corner,grid_side_dimensions->bottom_left_corner,grid_side_dimensions->bottom_right_corner);
}

void print_1d_array(char *array, int dimension) {
    for( int i = 0 ; i < dimension ; i++)
        printf("%c ",array[i]);
    printf("\n");
}

void free_2d_array(char ***array) {
    if( *array == NULL)
        return;
    free(&((*array)[0][0]));
    free(*array);
}

void free_grid(struct grid **grid) {
    if( *grid == NULL)
        return;
    free_2d_array(&(*grid)->array);
    free((*grid));
}

void free_grid_side_dimensions(struct grid_side_dimensions **grid_side_dimensions) {
    free((*grid_side_dimensions)->top_dimension);
    free((*grid_side_dimensions)->bottom_dimension);
    free((*grid_side_dimensions)->left_dimension);
    free((*grid_side_dimensions)->right_dimension);
    free((*grid_side_dimensions));
}