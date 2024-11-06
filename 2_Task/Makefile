CC = gcc
CFLAGS = -Wall -Wextra -fopenmp

all: merge_sort run_merge_sort

merge_sort: merge_sort.c 
	$(CC) $(CFLAGS) -o merge_sort merge_sort.c

run_merge_sort: merge_sort
	./merge_sort input.txt 4

clean:
	rm -f merge_sort sorted_par.txt sorted_seq.txt