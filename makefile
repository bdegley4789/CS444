all: run 

run: concurrency1.c
	gcc -pthread concurrency1.c -o run 


