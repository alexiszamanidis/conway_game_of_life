#include "../../header/header.h"
#include "../../header/grid.h"
#include "../../header/utilities.h"

int main( int argc, char **argv ) {

    srand(time(NULL));
    struct arguments arguments = (struct arguments) { .dimention = 10, .loops = 10, .filename = "\0", .output = false };

    parse_arguments(&arguments,argc,argv);

    print_arguments(arguments);

    struct grid *grid = allocate_grid(arguments.dimention);
    initialize_grid_from_inputfile(&grid,"../inputfiles/grid_10x10");
    
    print_grid(grid,"output");

    free_grid(&grid);

    return SUCCESS;
}