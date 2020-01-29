
mpicc -O3 -g -Wall -L/opt/ohpc/pub/libs/gnu8/openmpi3/mpiP/3.4.1/lib -lmpiP -lbfd -lunwind -c grid.c
mpicc -O3 -g -Wall -L/opt/ohpc/pub/libs/gnu8/openmpi3/mpiP/3.4.1/lib -lmpiP -lbfd -lunwind -c utilities.c
mpicc -O3 -g -Wall -L/opt/ohpc/pub/libs/gnu8/openmpi3/mpiP/3.4.1/lib -lmpiP -lbfd -lunwind grid.o utilities.o main.c -o game_of_life -lm
rm -f grid.o utilities.o
echo "Executable: 'game_of_life'."