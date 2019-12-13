## Table of Contents

* [Conway's game of life](https://github.com/ZamanidisAlexios/conway_game_of_life#conway's-game-of-life)

  * [Rules](https://github.com/ZamanidisAlexios/conway_game_of_life#rules)

  * [Implementation](https://github.com/ZamanidisAlexios/conway_game_of_life#implementation)

    * [Grid](https://github.com/ZamanidisAlexios/conway_game_of_life#grid)

# Conway's game of life

The Game of Life, also known simply as Life, is a cellular automaton devised by the British mathematician John Horton Conway in 1970. The game is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves.

## Rules

The universe of the Game of Life is an infinite, two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, alive or dead, (or populated and unpopulated, respectively). Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur: 

1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
2. Any live cell with two or three live neighbours lives on to the next generation.
3. Any live cell with more than three live neighbours dies, as if by overpopulation.
4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.

## Implementation

First of all, we pasre user's arguments, then we parse the number of processes because we want them to be a perfect square and the root to divide the global grid dimension with no remaining balance.

### Grid

After, process with rank equal to 0 allocates and initializes the global grid. The grid is a 2d array of characters. Characters have two values, the one is the dot(.) and the other is the star(*). Star means that the cell is alive and dot means that the cell is dead. We allocate the 2d grid contiguously, which means: