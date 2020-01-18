## Table of Contents

* [Conway's game of life](https://github.com/ZamanidisAlexios/conway_game_of_life#conways-game-of-life)

  * [Rules](https://github.com/ZamanidisAlexios/conway_game_of_life#rules)

  * [Mpi implementation](https://github.com/ZamanidisAlexios/conway_game_of_life#mpi-implementation)

    * [Grid](https://github.com/ZamanidisAlexios/conway_game_of_life#grid)

    * [Scattering](https://github.com/ZamanidisAlexios/conway_game_of_life#scattering)

    * [Neighbour calculation](https://github.com/ZamanidisAlexios/conway_game_of_life#neighbour-calculation)

    * [Central iteration](https://github.com/ZamanidisAlexios/conway_game_of_life#central-iteration)

# Conway's game of life

The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970. The game is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves.

## Rules

The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, alive or dead, (or populated and unpopulated, respectively). Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur: 

1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

## Mpi implementation

First of all, we pasre user's arguments, then we parse the number of processes because we want them to be a perfect square and the root to divide the global grid dimension with no remaining balance.

### Grid

After, process with rank equal to 0 allocates and initializes the global grid. The grid is a 2d array of characters. Characters have two values, the one is zero(0) and the other is one(1). One means that the cell is alive and zero means that the cell is dead. We allocate the 2d grid contiguously, which means:

![grid_allocation](https://user-images.githubusercontent.com/48658768/70812965-5898ca00-1dd1-11ea-893a-3ad410c0b58d.png)

Then, each process allocates a structure grid side dimensions, current generation and next generation. Structure grid side dimensions is used for saving the outline elements of the grid. Current generation and next generation are used for calculating and saving current local and next local grid cells.

## Scattering

After basic stucture allocation, we scatter the grid and we initialize all current generation grids. Example, if we have 4 processes, we will have:

![scattering](https://user-images.githubusercontent.com/48658768/70816300-de1f7880-1dd7-11ea-9dbb-22685ad61715.png)

## Neighbour calculation

Before the central iteration each process calculates the ranks of their neighbours, using MPI_Cart_create that makes a new communicator to which topology information has been attached. MPI_Cart_coords determines process coordinates in cartesian topology given rank in group, then with the help of MPI_Cart_rank we get all eight neighbours ranks.

![neighbour_calculation](https://user-images.githubusercontent.com/48658768/72661125-4b2dbb80-39df-11ea-9cbf-67e6a672bfa3.png)

As you can see from the picture, each process does eight requests(pink color) for specific outline elements using the ranks and sends their's eight outline elements(green color) to other neighbour processes.

### Central iteration

Our basic structure of central iteration is:

```
    MPI_Barrier
    Start MPI_Wtime
    For #iterations
        Irecv(RRequest)
        Isend(SRequest)
        Calculation_of_intermidiate_elements
        WaitAll
        Calculation_of_outline_elements
        (Gather_global_grid/Debugging)
        Swap_local_grids
        Inform_processes_about_looping
    End_for
    EndMPI_Wtime
```

Each process sends a variable to process with rank zero that informs if there was a difference between the two generations. If there was not any change on all processes, then the process with rank zero informs all other processes to stop looping.