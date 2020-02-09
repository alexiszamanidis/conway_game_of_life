## Table of Contents

* [Conway's game of life](https://github.com/ZamanidisAlexios/conway_game_of_life#conways-game-of-life)

  * [Rules](https://github.com/ZamanidisAlexios/conway_game_of_life#rules)
  
  * [Mpi implementation](https://github.com/ZamanidisAlexios/conway_game_of_life#mpi-implementation)
    * [Grid](https://github.com/ZamanidisAlexios/conway_game_of_life#grid)
    * [Scattering](https://github.com/ZamanidisAlexios/conway_game_of_life#scattering)
    * [Neighbour calculation](https://github.com/ZamanidisAlexios/conway_game_of_life#neighbour-calculation)
    * [Central iteration](https://github.com/ZamanidisAlexios/conway_game_of_life#central-iteration)

  * [Visualization](https://github.com/ZamanidisAlexios/conway_game_of_life#visualization)
  
  * [Benchmarking](https://github.com/ZamanidisAlexios/conway_game_of_life#benchmarking)
    * [Mpi measurements](https://github.com/ZamanidisAlexios/conway_game_of_life#mpi-measurements)
    * [Mpi OpenMp measurements](https://github.com/ZamanidisAlexios/conway_game_of_life#mpi-openmp-measurements)
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

### Scattering

After basic stucture allocation, we scatter the grid and we initialize all current generation grids. Example, if we have 4 processes, we will have:

![scattering](https://user-images.githubusercontent.com/48658768/70816300-de1f7880-1dd7-11ea-9dbb-22685ad61715.png)

### Neighbour calculation

Before the central iteration each process calculates the ranks of their neighbours, using MPI_Cart_create that makes a new communicator to which topology information has been attached. MPI_Cart_coords determines process coordinates in cartesian topology given rank in group, then with the help of MPI_Cart_rank we get all eight neighbours ranks.

![neighbour_calculation](https://user-images.githubusercontent.com/48658768/72804859-31e47380-3c5a-11ea-9ca5-a82395866b0e.png)

As you can see from the picture, each process does eight requests(pink color) for specific outline elements using the ranks and sends their's eight outline elements(green and blue color) to other neighbour processes.

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

Each process sends a variable to process with rank zero that informs if there was a difference between the two generations. If there was a difference between the two generations the variable will be one otherwise zero, so process zero adds all the variables with MPI Reduce. If there was not any change on all processes, the sum will be zero, then the process with rank zero will inform all other processes to stop looping.

## Visualization

You can run and see an interactive implementation of game of life, running the **conway_game_of_life.html** file.
It is a simple implementation that will help you to understand the rules and see some common patterns.
Many different types of patterns occur in the Game of Life, which are classified according to their behaviour.
Common pattern types include:

1. **Still lifes**, which do not change from one generation to the next.
2. **Oscillators**, which return to their initial state after a finite number of generations.
3. **Spaceships**, which translate themselves across the grid.

If the game ends or stucks to some pattern you can click on the logo and it will restart with a new random grid.

## Benchmarking

A benchmark is a test that measures the performance of hardware, software, or computer. These tests can be used to help compare how well a product may do against other products. When comparing benchmarks, the higher the value of the results, the faster the component, software, or overall computer is.

### Mpi measurements

#### Execution time

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|-----------|----------|----------|----------|----------|
 840      | 0.752412  | 0.193130  | 0.138504  | 0.082561  | 0.078703 | 0.091018 | 0.091000 | 0.102858 |
 1680     | 3.002919  | 0.755565  | 0.374157  | 0.229508  | 0.168460 | 0.148978 | 0.140478 | 0.140119 |
 3360     | 12.088203 | 3.021306  | 1.380812  | 0.796646  | 0.534765 | 0.405058 | 0.325011 | 0.285601 |
 6720     | 48.171624 | 12.172924 | 5.480808  | 3.134355  | 2.008271 | 1.423471 | 1.063272 | 0.852721 |

#### Speed up

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 3.895884  | 5.432421  | 9.113407 | 9.560144 | 8.266629 | 8.268264 | 7.315056 |
 1680     | 1         | 3.974402  | 8.025826  |13.084158 |17.825710 |20.156796 |21.376436 |21.431204 |
 3360     | 1         | 4.000986  | 8.754416  |15.173871 |22.604702 |29.843142 |37.193214 |42.325497 |
 6720     | 1         | 3.957276  | 8.789146  |15.368911 |23.986616 |33.840958 |45.305080 |56.491661 |

#### Efficiency

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 0.973971  | 0.603602  | 0.569588 | 0.382406 | 0.229629 | 0.168740 | 0.114298 |
 1680     | 1         | 0.993600  | 0.891758  | 0.817760 | 0.713028 | 0.559911 | 0.436254 | 0.334863 |
 3360     | 1         | 1.000247  | 0.972713  | 0.948367 | 0.904188 | 0.828976 | 0.759045 | 0.661336 |
 6720     | 1         | 0.989319  | 0.976572  | 0.960557 | 0.959465 | 0.940027 | 0.924593 | 0.882682 |

### Mpi OpenMp measurements

#### Execution time

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|-----------|----------|----------|----------|----------|
 840      | 0.806762  | 0.161556  | 0.252912  | 0.742792  | 0.243998 | 0.418301 | 0.453777 | 0.671479 |
 1680     | 3.214399  | 0.606590  | 0.479427  | 0.287312  | 0.220350 | 0.617887 | 0.982859 | 1.057335 |
 3360     | 12.757267 | 2.000665  | 1.503296  | 0.888122  | 0.821218 | 0.519624 | 0.842192 | 1.045532 |
 6720     | 51.037622 | 7.082620  | 6.437420  | 4.149907  | 2.595890 | 2.121335 | 1.736726 | 1.690778 |

#### Speed up

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 2.475934  | 0.052629  | 0.027886 | 0.027905 | 0.027703 | 0.035824 | 0.026989 |
 1680     | 1         | 0.652518  | 0.069407  | 0.137468 | 0.137736 | 0.123870 | 0.097978 | 0.096446 |
 3360     | 1         | 1.849977  | 0.526927  | 0.436462 | 0.410676 | 0.375429 | 0.403220 | 0.422518 |
 6720     | 1         | 0.860031  | 0.415485  | 0.613281 | 0.585318 | 0.552469 | 0.660797 | 0.750735 |

#### Efficiency

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 0.618983  | 0.005848  | 0.001743 | 0.001116 | 0.000770 | 0.000731 | 0.000422 |
 1680     | 1         | 0.163130  | 0.007712  | 0.008592 | 0.005509 | 0.003441 | 0.002000 | 0.001507 |
 3360     | 1         | 0.462494  | 0.058547  | 0.027279 | 0.016427 | 0.010429 | 0.008229 | 0.006602 |
 6720     | 1         | 0.215008  | 0.046165  | 0.038330 | 0.023413 | 0.015346 | 0.013486 | 0.011730 |
