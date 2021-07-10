# flags 
CC=gcc
FILE=unsupervised_k_means
FLAGS=-Wall
VALGRIND_FLAGS=--leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=./memory_leaks_test.txt \

# only works with linux. I found it difficult to install it on my device
test_cluster_for_memory_leaks: cluster
	valgrind $(VALGRIND_FLAGS) ./$(FILE)

cluster: cluster.o 
	$(CC) -o $(FILE) $(FILE).o $(FLAGS)

cluster.o: unsupervised_k_means.c 
	$(CC) -c $(FILE).c $(FLAGS)

clean:
	rm $(FILE) *.o OUTPUT.TXT memory_leaks_test.txt