CC = mpicc
OPENMP = -fopenmp
CFLAGS = -g -Wall
MATHFLAG = -lm
EXEC = mpi mpi_openmp
OBJS = grid.o utilities.o
OUTPUT = current_generation next_generation global_grid

all: $(EXEC) usage rm_objectives

mpi: $(OBJS) mpi.c
	$(CC) $(CFLAGS) $(OBJS) mpi.c -o mpi $(MATHFLAG)

mpi_openmp: $(OBJS) mpi_openmp.c
	$(CC) $(OPENMP) $(CFLAGS) $(OBJS) mpi_openmp.c -o mpi_openmp $(MATHFLAG)

rm_objectives:
	rm -rf $(OBJS)

grid.o: ../source/grid.c 
	$(CC) $(CFLAGS) -c ../source/grid.c

utilities.o: ../source/utilities.c
	$(CC) $(CFLAGS) -c ../source/utilities.c

usage:
	@echo 'Usage of Game of Life:'
	@echo 'mpiexec -n number_of_processes ./mpi -d dimentions -g generations -i inputfile -o'
	@echo 'mpiexec -n number_of_processes ./mpi_openmp -d dimentions -g generations -i inputfile -t threads -o'

clean: 
	rm -rf $(OBJS) $(EXEC) $(OUTPUT)