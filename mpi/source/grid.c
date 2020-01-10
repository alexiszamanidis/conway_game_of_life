#include "../header/grid.h"

struct grid_side_dimensions *allocate_grid_side_dimensions(int dimension) {
    struct grid_side_dimensions *grid_side_dimensions = (struct grid_side_dimensions *)malloc(sizeof(struct grid_side_dimensions));
    grid_side_dimensions->top_dimension = (char *)malloc(dimension*sizeof(char));
    if( grid_side_dimensions->top_dimension == NULL ) {
        printf("allocate_grid_side_dimensions: %s\n",strerror(errno));
        exit(FAILURE);
    }
    grid_side_dimensions->bottom_dimension = (char *)malloc(dimension*sizeof(char));
    if( grid_side_dimensions->bottom_dimension == NULL ) {
        printf("allocate_grid_side_dimensions: %s\n",strerror(errno));
        exit(FAILURE);
    }
    grid_side_dimensions->left_dimension = (char *)malloc(dimension*sizeof(char));
    if( grid_side_dimensions->left_dimension == NULL ) {
        printf("allocate_grid_side_dimensions: %s\n",strerror(errno));
        exit(FAILURE);
    }
    grid_side_dimensions->right_dimension = (char *)malloc(dimension*sizeof(char));
    if( grid_side_dimensions->right_dimension == NULL ) {
        printf("allocate_grid_side_dimensions: %s\n",strerror(errno));
        exit(FAILURE);
    }

    return grid_side_dimensions;
}

char **allocate_2d_array(int dimension) {
    // allocate the rows
    char **array = (char **) malloc(dimension * sizeof(char*));
    if(array == NULL) {
        printf("allocate_2d_array: %s\n",strerror(errno));
        exit(FAILURE);
    }
    
    // allocate all array elements => contiguous allocation
    char *allocate_all_array_elements = (char *) malloc(dimension * dimension * sizeof(char));
    if(allocate_all_array_elements == NULL) {
        printf("allocate_2d_array: %s\n",strerror(errno));
        exit(FAILURE);
    }
    memset(allocate_all_array_elements, 'A',dimension * dimension * sizeof(char));
    // fix array rows
    for (int i = 0; i < dimension; i++)
        array[i] = &(allocate_all_array_elements[i * dimension]);

    return array;
}

struct grid *allocate_grid(int dimension) {
    struct grid *grid = (struct grid *)malloc(sizeof(struct grid));
    if( grid == NULL ) {
        printf("allocate_grid: %s\n",strerror(errno));
        exit(FAILURE);
    }

    grid->array = allocate_2d_array(dimension);
    grid->dimension = dimension;

    return grid;
}

void initialize_grid(struct grid **grid) {
    char array_content[] = { '1', '0' };
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
            if( line[j] != '\0')
                (*grid)->array[i][j] = line[j];
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

char apply_rules(char state, int neighbours) {
    // if current state is a dead cell and has exactly 3 neighbours then the state becomes a live cell
    if( (state == '0') && (neighbours == 3))
        return '1';
    // if current state is a live cell and has fewer than 2 or more than 3 neighbours then the state becomes a dead cell
    else if( (state == '1') && ((neighbours < 2) || (neighbours > 3)) )
        return '0';
    // otherwise, if the current state has 2 or 3 neighbors lives on to the next generation
    else
        return state;
}

void swap_grids(struct grid **current_generation,struct grid **next_generation) {
    struct grid *temp_grid;
    temp_grid = *current_generation;
    *current_generation = *next_generation;
    *next_generation = temp_grid;
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
            fprintf(file_pointer, "%c",grid->array[i][j]);
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