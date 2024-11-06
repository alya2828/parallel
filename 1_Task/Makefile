CC = gcc
CFLAGS = -Wall -Wextra

all: generator run_generator radix_sort_mpi run_radix_sort_mpi

generator: generator.c
	$(CC) $(CFLAGS) -o generator generator.c

radix_sort_mpi: radix_sort_mpi.c
	mpicc $(CFLAGS) -o radix_sort_mpi radix_sort_mpi.c

run_generator: generator
	./generator

run_radix_sort_mpi: radix_sort_mpi
	mpirun -np 2 ./radix_sort_mpi input.txt

clean:
	rm -f radix_sort_mpi generator sorted_par.txt sorted_seq.txt input.txt