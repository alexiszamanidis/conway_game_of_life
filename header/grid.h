#ifndef grid_h_
#define grid_h_

#include "./header.h"
#include "./utilities.h"

#ifdef MPI_OPENMP
#define MPI_OPENMP 1
#endif

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
void print_grid(struct grid *, int , char *, int );
void print_sendcounts_and_displs(int *, int *, int);
void print_grid_side_dimensions(struct grid_side_dimensions *, int , int);
void print_1d_array(char *, int );
void free_2d_array(char ***);
void free_grid(struct grid **);
void free_grid_side_dimensions(struct grid_side_dimensions **);

static inline char apply_rules(char state, int neighbours) {
    // if current state is a dead cell and has exactly 3 neighbours then the state becomes a live cell
    if( (state == 0) && (neighbours == 3))
        return 1;
    // if current state is a live cell and has fewer than 2 or more than 3 neighbours then the state becomes a dead cell
    else if( (state == 1) && ((neighbours < 2) || (neighbours > 3)) )
        return 0;
    // otherwise, if the current state has 2 or 3 neighbors lives on to the next generation
    else
        return state;
}

static inline void receive(struct grid_side_dimensions *grid_side_dimensions,struct grid *current_generation,struct neighbour_processes neighbour_processes,MPI_Request *request) {
    MPI_Irecv(grid_side_dimensions->bottom_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.top_neighbour_rank, 0,MPI_COMM_WORLD, &request[8]);
    MPI_Irecv(grid_side_dimensions->top_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.bottom_neighbour_rank, 0,MPI_COMM_WORLD, &request[9]);
    MPI_Irecv(grid_side_dimensions->right_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.left_neighbour_rank, 0,MPI_COMM_WORLD, &request[10]);
    MPI_Irecv(grid_side_dimensions->left_dimension, current_generation->dimension, MPI_CHAR, neighbour_processes.right_neighbour_rank, 0,MPI_COMM_WORLD, &request[11]);
    MPI_Irecv(&grid_side_dimensions->bottom_right_corner, 1, MPI_CHAR, neighbour_processes.top_left_neighbour_rank, 0, MPI_COMM_WORLD, &request[12]);
    MPI_Irecv(&grid_side_dimensions->bottom_left_corner, 1, MPI_CHAR, neighbour_processes.top_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[13]);
    MPI_Irecv(&grid_side_dimensions->top_right_corner, 1, MPI_CHAR, neighbour_processes.bottom_left_neighbour_rank, 0, MPI_COMM_WORLD, &request[14]);
    MPI_Irecv(&grid_side_dimensions->top_left_corner, 1, MPI_CHAR, neighbour_processes.bottom_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[15]);
}

static inline void send(struct grid *current_generation,struct neighbour_processes neighbour_processes,MPI_Datatype columns,MPI_Request *request) {
    MPI_Isend(&current_generation->array[0][0], current_generation->dimension, MPI_CHAR, neighbour_processes.top_neighbour_rank, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Isend(&current_generation->array[current_generation->dimension-1][0], current_generation->dimension, MPI_CHAR, neighbour_processes.bottom_neighbour_rank, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Isend(&current_generation->array[0][0], 1, columns, neighbour_processes.left_neighbour_rank, 0,MPI_COMM_WORLD, &request[2]);
    MPI_Isend(&current_generation->array[0][current_generation->dimension-1], 1, columns, neighbour_processes.right_neighbour_rank, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(&current_generation->array[0][0], 1, MPI_CHAR, neighbour_processes.top_left_neighbour_rank, 0,MPI_COMM_WORLD, &request[4]);
    MPI_Isend(&current_generation->array[0][current_generation->dimension-1], 1, MPI_CHAR, neighbour_processes.top_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Isend(&current_generation->array[current_generation->dimension-1][0], 1, MPI_CHAR, neighbour_processes.bottom_left_neighbour_rank, 0,MPI_COMM_WORLD, &request[6]);
    MPI_Isend(&current_generation->array[current_generation->dimension-1][current_generation->dimension-1],1, MPI_CHAR, neighbour_processes.bottom_right_neighbour_rank, 0, MPI_COMM_WORLD, &request[7]);
}

static inline void calculate_intermidiate_elements(struct grid *current_generation,struct grid *next_generation, int *different_generations) {
    int i, j, neighbours=0;
#if MPI_OPENMP
    #pragma omp parallel for shared (current_generation, next_generation, different_generations) private(i, j) reduction(+:neighbours) collapse(2)
#endif
    for( i = 1 ; i < current_generation->dimension-1 ; i++ ) {
        for( j = 1 ; j < current_generation->dimension-1 ; j++ ) {
            // sum all alive neighbours
            neighbours = (current_generation->array[i-1][j-1])+(current_generation->array[i-1][j])+(current_generation->array[i-1][j+1])
                        +(current_generation->array[i][j-1])+(current_generation->array[i][j+1])
                        +(current_generation->array[i+1][j-1])+(current_generation->array[i+1][j])+(current_generation->array[i+1][j+1]);
            // apply the rules and create next generation grid
            next_generation->array[i][j] = apply_rules(current_generation->array[i][j],neighbours);

            // check if there is a difference between the generations
            if( (*different_generations == 0) && (next_generation->array[i][j] != current_generation->array[i][j]) )
                *different_generations = 1;
        }
    }
}

static inline void calculate_outline_elements(struct grid *current_generation,struct grid *next_generation, struct grid_side_dimensions *grid_side_dimensions,int *different_generations, int last) {
    int i, neighbours=0;
#if MPI_OPENMP
    #pragma omp parallel for shared (current_generation, next_generation, grid_side_dimensions, different_generations) private(i) reduction(+:neighbours)
#endif
    for( i = 1 ; i < current_generation->dimension-1 ; i++ ) {
        // top dimension: sum all alive neighbours
        neighbours = (current_generation->array[0][i-1])+(current_generation->array[0][i+1])
                    +(current_generation->array[1][i-1])+(current_generation->array[1][i])+(current_generation->array[1][i+1])
                    +(grid_side_dimensions->top_dimension[i-1])+(grid_side_dimensions->top_dimension[i])+(grid_side_dimensions->top_dimension[i+1]);
        // apply the rules and create next generation grid
        next_generation->array[0][i] = apply_rules(current_generation->array[0][i],neighbours);

        // bottom dimension: sum all alive neighbours
        neighbours = (current_generation->array[last][i-1])+(current_generation->array[last][i+1])
                    +(current_generation->array[last-1][i-1])+(current_generation->array[last-1][i])+(current_generation->array[last-1][i+1])
                    +(grid_side_dimensions->bottom_dimension[i-1])+(grid_side_dimensions->bottom_dimension[i])+(grid_side_dimensions->bottom_dimension[i+1]);
        // apply the rules and create next generation grid
        next_generation->array[last][i] = apply_rules(current_generation->array[last][i],neighbours);

        // left dimension: sum all alive neighbours
        neighbours = (current_generation->array[i-1][0])+(current_generation->array[i+1][0])
                    +(current_generation->array[i-1][1])+(current_generation->array[i][1])+(current_generation->array[i+1][1])
                    +(grid_side_dimensions->left_dimension[i-1])+(grid_side_dimensions->left_dimension[i])+(grid_side_dimensions->left_dimension[i+1]);
        // apply the rules and create next generation grid
        next_generation->array[i][0] = apply_rules(current_generation->array[i][0],neighbours);

        // right dimension: sum all alive neighbours
        neighbours = (current_generation->array[i-1][last])+(current_generation->array[i+1][last])
                    +(current_generation->array[i-1][last-1])+(current_generation->array[i][last-1])+(current_generation->array[i+1][last-1])
                    +(grid_side_dimensions->right_dimension[i-1])+(grid_side_dimensions->right_dimension[i])+(grid_side_dimensions->right_dimension[i+1]);
        // apply the rules and create next generation grid
        next_generation->array[i][last] = apply_rules(current_generation->array[i][last],neighbours);

        // check if there is a difference between the generations
        if( *different_generations == 0 ) {
            if( (next_generation->array[0][i] != current_generation->array[0][i]) || (next_generation->array[last][i] != current_generation->array[last][i]) ||
                (next_generation->array[i][0] != current_generation->array[i][0]) || (next_generation->array[i][last] != current_generation->array[i][last]) ) {
                *different_generations = 1;
            }
        }
    }
    // top left corner cell: sum all alive neighbours
    neighbours = (current_generation->array[0][1])+(current_generation->array[1][0])+(current_generation->array[1][1])
                +(grid_side_dimensions->top_dimension[0])+(grid_side_dimensions->top_dimension[1])
                +(grid_side_dimensions->left_dimension[0])+(grid_side_dimensions->left_dimension[1])
                +(grid_side_dimensions->top_left_corner);
    // apply the rules and create next generation grid
    next_generation->array[0][0] = apply_rules(current_generation->array[0][0],neighbours);

    // top right corner cell: sum all alive neighbours
    neighbours = (current_generation->array[0][last-1])+(current_generation->array[1][last])+(current_generation->array[1][last-1])
                +(grid_side_dimensions->top_dimension[last])+(grid_side_dimensions->top_dimension[last-1])
                +(grid_side_dimensions->right_dimension[0])+(grid_side_dimensions->right_dimension[1])
                +(grid_side_dimensions->top_right_corner);
    // apply the rules and create next generation grid
    next_generation->array[0][last] = apply_rules(current_generation->array[0][last],neighbours);

    // bottom left corner cell: sum all alive neighbours
    neighbours = (current_generation->array[last-1][0])+(current_generation->array[last-1][1])+(current_generation->array[last][1])
                +(grid_side_dimensions->bottom_dimension[0])+(grid_side_dimensions->bottom_dimension[1])
                +(grid_side_dimensions->left_dimension[last])+(grid_side_dimensions->left_dimension[last-1])
                +(grid_side_dimensions->bottom_left_corner);
    // apply the rules and create next generation grid
    
    next_generation->array[last][0] = apply_rules(current_generation->array[last][0],neighbours);
    // bottom right corner cell: sum all alive neighbours
    neighbours = (current_generation->array[last-1][last])+(current_generation->array[last-1][last-1])+(current_generation->array[last][last-1])
                +(grid_side_dimensions->bottom_dimension[last])+(grid_side_dimensions->bottom_dimension[last-1])
                +(grid_side_dimensions->right_dimension[last])+(grid_side_dimensions->right_dimension[last-1])
                +(grid_side_dimensions->bottom_right_corner);
    // apply the rules and create next generation grid
    next_generation->array[last][last] = apply_rules(current_generation->array[last][last],neighbours);

    // check if there is a difference between the generations
    if( *different_generations == 0 ) {
        if( (next_generation->array[0][0] != current_generation->array[0][0]) || (next_generation->array[0][last] != current_generation->array[0][last]) ||
            (next_generation->array[last][0] != current_generation->array[last][0]) || (next_generation->array[last][last] != current_generation->array[last][last]) ) {
            *different_generations = 1;
        }
    }
}

#endif