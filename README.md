## Table of Contents

* [Conway's game of life](https://github.com/ZamanidisAlexios/conway_game_of_life#conways-game-of-life)
  * [Rules](https://github.com/ZamanidisAlexios/conway_game_of_life#rules)
  * [Foster's Design Methology](https://github.com/ZamanidisAlexios/conway_game_of_life#fosters-design-methology)
  * [Visualization](https://github.com/ZamanidisAlexios/conway_game_of_life#visualization)
  * [Execution instructions](https://github.com/ZamanidisAlexios/conway_game_of_life#execution-instructions)

# Conway's game of life

The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton 
Conway in 1970. The game is a zero-player game, meaning that its evolution is determined by its initial state, 
requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves.

## Rules

The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells, each of which is in 
one of two possible states, alive or dead, (or populated and unpopulated, respectively). 
Every cell interacts with its eight neighbours, which are the cells that are horizontally, 
vertically, or diagonally adjacent. At each step in time, the following transitions occur: 

1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

## Foster's Design Methology

I followed Foster's Design Methology to parallelize the game of life. 
It is a four-stage design process:

1. **Partitioning**: The process of dividing the computation and the data into pieces.
2. **Communication**: The process of determining how tasks will communicate with each other, distinguishing
between local communication and global communication.
3. **Agglomeration**: The process of grouping tasks into larger tasks to improve performance or simplify programming.
4. **Mapping**: The process of assigning tasks to physical processors.

![foster_design_methology](https://user-images.githubusercontent.com/48658768/87170283-cf4f6480-c2d9-11ea-918c-50c8229fec07.png)

## Visualization

You can run and see an interactive implementation of game of life, running the **conway_game_of_life.html** file.
It is a simple implementation that will help you to understand the rules and see some common patterns.
Many different types of patterns occur in the Game of Life, which are classified according to their behaviour.
Common pattern types include:

1. **Still lifes**, which do not change from one generation to the next.
2. **Oscillators**, which return to their initial state after a finite number of generations.
3. **Spaceships**, which translate themselves across the grid.

If the game ends or stucks to some pattern you can click on the logo and it will restart with a new random grid.

## Execution instructions

An example of how you can run game of life:

```
    $ cd source
    $ make
    $ mpiexec -n 4 ./mpi -d 10 -g 2 -i ../inputfiles/grid_10x10.csv
    $ mpiexec -n 4 ./mpi_openmp -d 10 -g 2 -i ../inputfiles/grid_10x10.csv -t 2
```