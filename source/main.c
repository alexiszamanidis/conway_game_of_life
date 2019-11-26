#include "../header/header.h"
#include "../header/grid.h"

int main( int argc, char **argv ) {

    srand(time(NULL));

    struct grid *grid = allocate_grid(5);
    initialize_grid(&grid);
    
    print_grid(grid,"output");

    free_grid(&grid);

    return SUCCESS;
}