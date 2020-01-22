let grid;
let dimension;
let resolution = 10;

function allocate_grid(dimension) {
    let grid = new Array(dimension);
    for (let i = 0; i < grid.length; i++)
        grid[i] = new Array(dimension);

    return grid;
}

function setup() {
    let canvas = createCanvas(600, 600);
    canvas.center();
    dimension = width / resolution;
    grid = allocate_grid(dimension);
    // initialize first grid
    for (let i = 0 ; i < dimension ; i++)
        for (let j = 0 ; j < dimension ; j++)
            grid[i][j] = floor(random(2));
}

function draw() {
    background(1);

    // draw grid
    for (let i = 0 ; i < dimension ; i++) {
        for (let j = 0 ; j < dimension ; j++) {
            let x = i * resolution;
            let y = j * resolution;
            // if grid[i][j] = 0, then fill rectangular with white
            if( grid[i][j] == 0 )
                fill(255);
            // if grid[i][j] = 1, then fill rectangular with black
            else
                fill(1);
            stroke(100);
            rect(x, y, resolution - 1, resolution - 1);
        }
    }

    // allocate next grid
    let next_grid = allocate_grid(dimension);

    // calculate next grid
    for (let i = 0 ; i < dimension ; i++) {
        for (let j = 0 ; j < dimension ; j++) {
            let neighbours = calculate_neighbours(i,j);
            next_grid[i][j] = apply_rules(grid[i][j],neighbours);
        }
    }
    grid = next_grid;
}

function calculate_neighbours(x,y) {
    let neighbours = 0;
    for (let i = -1 ; i < 2 ; i++) {
        for (let j = -1 ; j < 2 ; j++) {
            if( (i == 0) && (j ==0) )
                continue;
            let temp_x = (x + i + dimension) % dimension;
            let temp_y = (y + j + dimension) % dimension;
            neighbours = neighbours + grid[temp_x][temp_y];
        }
    }
    return neighbours;
}

function apply_rules(state, neighbours) {
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