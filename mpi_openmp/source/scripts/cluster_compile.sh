module load mpiP

echo "mpicc -O3 -g -fopenmp -Wall -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -c ../grid.c";
if ! mpicc -O3 -g -fopenmp -Wall -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -c ../grid.c 2> /dev/null
then
    echo "ERROR grid.c failed to compile"; exit 1
fi

echo "mpicc -O3 -g -fopenmp -Wall -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -c ../utilities.c";
if ! mpicc -O3 -g -fopenmp -Wall -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -c ../utilities.c 2> /dev/null
then
    echo "ERROR utilities.c failed to compile"; exit 1
fi

echo "mpicc -O3 -g -fopenmp -Wall grid.o utilities.o main.c -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -o game_of_life -lm";
if ! mpicc -O3 -g -fopenmp -Wall grid.o utilities.o ../main.c -L$MPIP_DIR/lib -lmpiP -lbfd -lunwind -o game_of_life -lm 2> /dev/null
then
    echo "ERROR main.c failed to compile"; exit 1
fi

rm -rf *.o

echo "Executable: 'game_of_life'."