## Table of Contents

* [Mpi implementation](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#mpi-implementation)
  * [Grid](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#grid)
  * [Scattering](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#scattering)
  * [Neighbour calculation](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#neighbour-calculation)
  * [Central iteration](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#central-iteration)
* [Mpi profiling](https://github.com/ZamanidisAlexios/conway_game_of_life/tree/master/source#mpi-profiling)

## Mpi implementation

First of all, we pasre user's arguments, then we parse the number of processes because we want them to be a perfect 
square and the root to divide the global grid dimension with no remaining balance.

### Grid

After, process with rank equal to 0 allocates and initializes the global grid. The grid is a 2d array of characters. 
Characters have two values, the one is zero(0) and the other is one(1). One means that the cell is alive and zero means 
that the cell is dead. We allocate the 2d grid contiguously, which means:

![grid_allocation](https://user-images.githubusercontent.com/48658768/70812965-5898ca00-1dd1-11ea-893a-3ad410c0b58d.png)

Then, each process allocates a structure grid side dimensions, current generation and next generation. Structure grid side 
dimensions is used for saving the outline elements of the grid. Current generation and next generation are used for calculating 
and saving current local and next local grid cells.

### Scattering

After basic stucture allocation, we scatter the grid and we initialize all current generation grids. Example, if we have 4 
processes, we will have:

![scattering](https://user-images.githubusercontent.com/48658768/70816300-de1f7880-1dd7-11ea-9dbb-22685ad61715.png)

### Neighbour calculation

Before the central iteration each process calculates the ranks of their neighbours, using MPI_Cart_create that makes a new 
communicator to which topology information has been attached. MPI_Cart_coords determines process coordinates in cartesian 
topology given rank in group, then with the help of MPI_Cart_rank we get all eight neighbours ranks.

![neighbour_calculation](https://user-images.githubusercontent.com/48658768/79388040-58722c00-7f75-11ea-9d33-5b6cfb738663.png)

As you can see from the picture, each process does eight requests(pink color) for specific outline elements using the ranks 
and sends their's eight outline elements(green and blue color) to other neighbour processes.

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

Each process sends a variable to process with rank zero that informs if there was a difference between the two generations. 
If there was a difference between the two generations the variable will be one otherwise zero, so process zero adds all the 
variables with MPI Reduce. If there was not any change on all processes, the sum will be zero, then the process with rank 
zero will inform all other processes to stop looping.

## Visualization

You can run and see an interactive implementation of game of life, running the **conway_game_of_life.html** file.
It is a simple implementation that will help you to understand the rules and see some common patterns.
Many different types of patterns occur in the Game of Life, which are classified according to their behaviour.
Common pattern types include:

1. **Still lifes**, which do not change from one generation to the next.
2. **Oscillators**, which return to their initial state after a finite number of generations.
3. **Spaceships**, which translate themselves across the grid.

If the game ends or stucks to some pattern you can click on the logo and it will restart with a new random grid.

## Mpi profiling

This next section provides an overview of the application's time in MPI. Apptime is the wallclock time from the end of MPI_Init until the beginning of MPI_Finalize. MPI_Time is the wall-clock time for all the MPI calls contained within Apptime. MPI% shows the ratio of this MPI_Time to Apptime. The asterisk (*) is the aggregate line for the entire application.

![mpi_time](https://user-images.githubusercontent.com/48658768/74974713-6badc300-542e-11ea-93b0-f1520c9eded4.png)

The callsite section identifies all the MPI callsites within the application. The first number is the callsite ID for this mpiP file. The next column shows the type of MPI call (w/o the MPI_ prefix). The name of the function that contains this MPI call is next, followed by the file name and line number. Finally, the last column shows the PC, or program counter, for that MPI callsite. Note that the default setting for callsite stack walk depth is 1. Other settings will enumerate callsites by the entire stack trace rather than the single callsite alone.

![mpi_callsites](https://user-images.githubusercontent.com/48658768/74974724-723c3a80-542e-11ea-82ee-5ea57a675867.png)

The aggregate time section is a very quick overview of the top twenty MPI callsites that consume the most aggregate time in your application. Call identifies the type of MPI function. Site provides the callsite ID (as listed in the callsite section). Time is the aggregate time for that callsite in milliseconds. The next two columns show the ratio of that aggregate time to the total application time and to the total MPI time, respectively. The COV column indicates the variation in times of individual processes for this callsite by presenting the coefficient of variation as calculated from the individual process times. A larger value indicates more variation between the process times.

![mpi_aggregate_time](https://user-images.githubusercontent.com/48658768/74974731-75372b00-542e-11ea-93ab-e5340fe95051.png)

The next section is similar to the aggregate time section, although it reports on the top 20 callsites for total sent message sizes.

![mpi_aggregate_sent_message_size](https://user-images.githubusercontent.com/48658768/74974743-79fbdf00-542e-11ea-9298-1e0153194937.png)