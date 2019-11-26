CC = mpicc
CFLAGS = -g -Wall -lm
EXEC = game_of_life
OBJS = grid.o utilities.o
OUTPUT = output

all: main usage

# main program
main: $(OBJS) main.c 
	$(CC) $(CFLAGS) $(OBJS) main.c -o $(EXEC)
	rm -rf $(OBJS)

grid.o: grid.c 
	$(CC) $(CFLAGS) -c grid.c

utilities.o: utilities.c
	$(CC) $(CFLAGS) -c utilities.c

usage:
	@echo 'Usage of Project: valgrind ./game_of_life'
clean: 
	rm -f $(OBJS) $(EXEC) $(OUTPUT)
