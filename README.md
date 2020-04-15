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
  * [Mpi profiling](https://github.com/ZamanidisAlexios/conway_game_of_life#mpi-profiling)

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

![neighbour_calculation](https://user-images.githubusercontent.com/48658768/79388040-58722c00-7f75-11ea-9d33-5b6cfb738663.png)

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

![mpi_execution_time](https://user-images.githubusercontent.com/48658768/74972536-94cc5480-542a-11ea-8039-fedb03cd7343.png)
![mpi_speed_up](https://user-images.githubusercontent.com/48658768/74972132-e2948d00-5429-11ea-805f-dd71f4800f39.png)
![mpi_efficiency](https://user-images.githubusercontent.com/48658768/74972139-e3c5ba00-5429-11ea-91e6-e7107bf8bd5d.png)

The ideal value for Speed-up/S(n,p) is p. If S(n,p) = p, then the parallel program with comm_sz = p processes is executed 
p times faster than the serial program. So we can see from our Speed-up table and graph that for small number of Processes 
and big number of Dimension our program has almost serial Speed-up. But, for big number of Processes and small number of 
Dimension our Speed-up is too small. Also, we have very good and decent Speed-up for big number of Processes 
and big number of Dimension.
Εfficiency is the Speed-up / Processes, so if we have a serial Speed-up our parallel Εfficiency will be p/p = 1. 
So we can see from our Εfficiency table and graph that for small number of Processes and big number of Dimension our 
Εfficiency is almost serial Speed-up. But, for big number of Processes and small number of Dimension our Εfficiency 
is too small.

### Mpi OpenMp measurements( 8-threads )

#### Execution time

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25        | 36       | 49       | 64       |
----------|-----------|-----------|-----------|-----------|-----------|----------|----------|----------|
 840      | 0.803773  | 0.225603  | 0.140552  | 0.203057  | 0.151474  | 0.650479 | 0.511546 | 0.659165 |
 1680     | 3.201418  | 0.640185  | 0.401929  | 0.268558  | 0.563328  | 0.698334 | 1.330187 | 0.993040 |
 3360     | 12.733003 | 2.032802  | 1.475748  | 0.876268  | 0.763466  | 0.664128 | 0.880283 | 1.209686 |
 6720     | 50.990898 | 7.043701  | 6.439718  | 3.924058  | 2.743390  | 1.910961 | 1.500764 | 1.401790 |

#### Speed up

 Dmnsn\Pro| 1         | 4         | 9         | 16        | 25       | 36        | 49        | 64        |
----------|-----------|-----------|-----------|-----------|----------|-----------|-----------|-----------|
 840      | 1         | 3.562776  | 5.718688  | 3.958361  | 5.306343 | 1.235663  | 1.571262  | 1.219380  |
 1680     | 1         | 5.000770  | 7.965133  | 11.920770 | 5.683044 | 4.584365  | 2.406743  | 3.223856  |
 3360     | 1         | 6.263769  | 8.628169  | 14.530945 |16.677891 | 19.172512 | 14.464669 | 10.525874 |
 6720     | 1         | 7.239219  | 7.918189  | 12.994431 |18.586821 | 26.683380 | 33.976627 | 36.375561 |

#### Efficiency

 Dmnsn\Pro| 1         | 4         | 9         | 16       | 25       | 36       | 49       | 64       |
----------|-----------|-----------|-----------|----------|----------|----------|----------|----------|
 840      | 1         | 0.890694  | 0.635410  | 0.247398 | 0.212254 | 0.034324 | 0.032067 | 0.019053 |
 1680     | 1         | 1.250193  | 0.885015  | 0.745048 | 0.227322 | 0.127343 | 0.049117 | 0.050373 |
 3360     | 1         | 1.565942  | 0.958685  | 0.908184 | 0.667116 | 0.532570 | 0.295197 | 0.164467 |
 6720     | 1         | 1.809805  | 0.879799  | 0.812152 | 0.743473 | 0.741205 | 0.693401 | 0.568368 |

![mpi_openmp_execution_time](https://user-images.githubusercontent.com/48658768/74972148-e6c0aa80-5429-11ea-8b40-f8f2891ba14d.png)
![mpi_openmp_speed_up](https://user-images.githubusercontent.com/48658768/74972155-e9bb9b00-5429-11ea-901b-67adb6750192.png)
![mpi_openmp_efficiency](https://user-images.githubusercontent.com/48658768/74972161-eb855e80-5429-11ea-9d06-d4ecb903273e.png)

So we can see from our Speed-up table and graph that for four processes we get double Speed-up. Μore generally, 
we can conclude that only for small Processes we have good Speed-up. But, for big number of Processes and big number of 
Dimension our Speed-up is too small.
From our Εfficiency table and graph, we can see that for small number of Processes and big number of Dimension our 
Εfficiency is almost one or even greater than one for four Processes. But, for big number of Processes our Εfficiency 
is too small.

## Mpi profiling

This next section provides an overview of the application's time in MPI. Apptime is the wallclock time from the end of MPI_Init until the beginning of MPI_Finalize. MPI_Time is the wall-clock time for all the MPI calls contained within Apptime. MPI% shows the ratio of this MPI_Time to Apptime. The asterisk (*) is the aggregate line for the entire application.

![mpi_time](https://user-images.githubusercontent.com/48658768/74974713-6badc300-542e-11ea-93b0-f1520c9eded4.png)

The callsite section identifies all the MPI callsites within the application. The first number is the callsite ID for this mpiP file. The next column shows the type of MPI call (w/o the MPI_ prefix). The name of the function that contains this MPI call is next, followed by the file name and line number. Finally, the last column shows the PC, or program counter, for that MPI callsite. Note that the default setting for callsite stack walk depth is 1. Other settings will enumerate callsites by the entire stack trace rather than the single callsite alone.

![mpi_callsites](https://user-images.githubusercontent.com/48658768/74974724-723c3a80-542e-11ea-82ee-5ea57a675867.png)

The aggregate time section is a very quick overview of the top twenty MPI callsites that consume the most aggregate time in your application. Call identifies the type of MPI function. Site provides the callsite ID (as listed in the callsite section). Time is the aggregate time for that callsite in milliseconds. The next two columns show the ratio of that aggregate time to the total application time and to the total MPI time, respectively. The COV column indicates the variation in times of individual processes for this callsite by presenting the coefficient of variation as calculated from the individual process times. A larger value indicates more variation between the process times.

![mpi_aggregate_time](https://user-images.githubusercontent.com/48658768/74974731-75372b00-542e-11ea-93ab-e5340fe95051.png)

The next section is similar to the aggregate time section, although it reports on the top 20 callsites for total sent message sizes.

![mpi_aggregate_sent_message_size](https://user-images.githubusercontent.com/48658768/74974743-79fbdf00-542e-11ea-9298-1e0153194937.png)