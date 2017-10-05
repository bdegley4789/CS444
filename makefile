all: concurrency1 mt19937ar

concurrency1: concurrency1.c
	gcc -o concurrency1 concurrency1.c

mt19937: mt19937ar.c
	gcc -o mt19937 mt19937ar.c
