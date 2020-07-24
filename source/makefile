CC = mpicc -fopenmp #-DGOL_DEBUG
CFLAGS = -g -Wall
MATHFLAG = -lm
EXEC = mpi mpi_openmp
OBJS = grid.o utilities.o
OUTPUT = current_generation next_generation global_grid

all: $(EXEC) usage rm_output
	rm -rf $(OBJS)

mpi: $(OBJS) main.c
	$(CC) $(CFLAGS) $(OBJS) main.c -o mpi $(MATHFLAG)

mpi_openmp: $(OBJS) main.c
	$(CC) $(CFLAGS) -DMPI_OPENMP $(OBJS) main.c -o mpi_openmp $(MATHFLAG)

rm_output:
	rm -rf $(OUTPUT)

grid.o: ../source/grid.c 
	$(CC) $(CFLAGS) -c ../source/grid.c

utilities.o: ../source/utilities.c
	$(CC) $(CFLAGS) -c ../source/utilities.c

usage:
	@echo 'Usage of Game of Life:'
	@echo 'mpiexec -n number_of_processes ./mpi -d dimentions -g generations -i inputfile'
	@echo 'mpiexec -n number_of_processes ./mpi_openmp -d dimentions -g generations -i inputfile -t threads'

clean: 
	rm -rf $(OBJS) $(EXEC) $(OUTPUT)