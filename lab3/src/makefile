CC = gcc
CFLAGS = -I.

all: sequential_min_max parallel_min_max

fork_seq: fork_seq.o utils.o find_min_max.o
	$(CC) -o fork_seq fork_seq.o utils.o find_min_max.o $(CFLAGS)

fork_seq.o: fork_seq.c
	$(CC) -c -o fork_seq.o fork_seq.c $(CFLAGS)

sequential_min_max: sequential_min_max.o utils.o find_min_max.o
	$(CC) -o sequential_min_max sequential_min_max.o utils.o find_min_max.o $(CFLAGS)

parallel_min_max: parallel_min_max.o utils.o find_min_max.o
	$(CC) -o parallel_min_max parallel_min_max.o utils.o find_min_max.o $(CFLAGS)

sequential_min_max.o: sequential_min_max.c utils.h find_min_max.h
	$(CC) -c -o sequential_min_max.o sequential_min_max.c $(CFLAGS)

parallel_min_max.o: parallel_min_max.c utils.h find_min_max.h
	$(CC) -c -o parallel_min_max.o parallel_min_max.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c -o utils.o utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c -o find_min_max.o find_min_max.c $(CFLAGS)

clean:
	rm -f *.o sequential_min_max parallel_min_max fork_seq

.PHONY: all clean