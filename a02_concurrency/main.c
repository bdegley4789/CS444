//Bryce Egley and Bruce Garcia
//CS 444
//Concurrency exercise 2: The Dining Philosophers Problem
//We re-used some of the code from concurrency exercise #1
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.c"
#include <assert.h>
#define maxSize 5 //Table will hold 5 plates and forks

//Information on building structs used and modified from the following
//https://stackoverflow.com/questions/32698293
typedef struct data {
    char[9] name;
} Data;

//Array of forks on table
Data arrTable[maxSize];

//Array data structure of 32 size
int count = 0;

typedef struct status {
    char[12] status;
} Status;

Status arrPhilosophers[maxSize];

int phil_count = 0;

pthread_cond_t thread_plato, thread_aristotle, thread_socrates, thread_confucius, thread_epicurus;
pthread_mutex_t thread_mutex;

void think() {
    //Think for 1-20 seconds
    unsigned int think_sleep = (genrand_int32() % 20) + 1;
    while(think_sleep)
        think_sleep = sleep(think_sleep);
}
void eat() {
    //Eat for 2-9 seconds
    unsigned int eat_sleep = (genrand_int32() % 8) + 2;
    while(eat_sleep)
        eat_sleep = sleep(eat_sleep);
}
void get_forks(char philospher_name[9], pthread_cond_t phil) {
    
    //Getting fork takes between 1-3 seconds
	unsigned int getting_fork = (genrand_int32() % 3) + 1;
    //Change philospher status
    Status p2;
    p2 = (Status){.status = "Getting Fork"};
    pthread_mutex_lock(&thread_mutex);
    arrPhilosophers[phil_count] = p2;
    phil_count++;
    //Print new status
    print_status();
    pthread_mutex_unlock(&thread_mutex);
	// Check to see if there are any forks available
    pthread_mutex_lock(&thread_mutex);
    while(count >= maxSize) {
        printf("ERROR: No Forks available...\n");
        pthread_cond_wait(&thread_mutex);
    }
    pthread_mutex_unlock(&thread_mutex);
    //Build new data
    Data p1;
    p1 = (Data){.name = philospher_name};
    //Change Status
    p2 = (Status){.status = "Eating"};
    arrPhilosophers[phil_count] = p2;
    phil_count--;
    //Lock Mutex for thread use
    pthread_mutex_lock(&thread_mutex);
    //Add new philospher name to forks
    arrTable[count] = p1;
    count++;
    pthread_cond_signal(&phil);
    ////Unlock mutex so other threads can use data structure
    pthread_mutex_unlock(&thread_mutex);
    //Output new results
    print_status();
}
void put_forks(char philospher_name[9], pthread_cond_t phil) {
    
    //Putting fork takes between 1-3 seconds
	unsigned int putting_fork = (genrand_int32() % 3) + 1;
	//Return the fork
    pthread_mutex_lock(&thread_mutex);
    while (count <= 0) {
        printf("Error: No place to put fork down...\n");
        pthread_cond_wait(&thread_mutex);
    }
    pthread_mutex_unlock(&thread_mutex);
    pthread_mutex_lock(&thread_mutex);
    //Need to check that specific thread is removed
    count--;
    pthread_cond_signal(&phil);
    //Unlock mutex so other threads can use data structure
    pthread_mutex_unlock(&thread_mutex);
    //Output new results
    print_status();
}

void print_results() {
    printf("***************************************");
    printf("Fork Status\n");
    for (int i = 0; i < maxSize; i++) {
        printf("Fork %d is currently held by %s\n",i+1,)
    }
    printf("Philosopher Status\n");
    for (int j = 0; j < maxSize; j++) {
        printf("%s is currently %s",);
    }
    printf("***************************************");
}

void *plato(void* ptr)
{
    char name[9] = "plato";
    while(1) {
        think();
        get_forks(name,thread_plato);
        eat();
        put_forks(name,thread_plato);
    }
}

void *aristotle(void* ptr)
{
    char name[9] = "aristotle";
    while(1) {
        think();
        get_forks(name,thread_aristotle);
        eat();
        put_forks(name,thread_aristotle);
    }
}

void *socrates(void* ptr)
{
    char name[9] = "socrates";
    while(1) {
        think();
        get_forks(name,thread_socrates);
        eat();
        put_forks(name,thread_socrates);
    }
}

void *confucius(void* ptr)
{
    char name[9] = "confucius";
    while(1) {
        think();
        get_forks(name,thread_confucius);
        eat();
        put_forks(name,thread_confucius);
    }
}

void *epicurus(void* ptr)
{
    char name[9] = "epicurus";
    while(1) {
        think();
        get_forks(name,thread_epicurus);
        eat();
        put_forks(name,thread_epicurus);
    }
}

int main() {
    //Have seed generate for random sequnce of numbers with genrand
    init_genrand(time(NULL));
    // Thread ID.
    pthread_t tidPlato;
    pthread_t tidAristotle;
    pthread_t tidSocrates;
    pthread_t tidConfucius;
    pthread_t tidEpicurus;
    
    //Create mutex so threads can both use shared resource
    if(pthread_mutex_init(&thread_mutex, NULL)) {
        fprintf(stderr, "Error creating mutex");
        return 1;
    }
    
    //Initialize conditional vars so thread can wait until condition occurs
    pthread_cond_init(&thread_plato, NULL);
    pthread_cond_init(&thread_aristotle, NULL);
    pthread_cond_init(&thread_socrates, NULL);
    pthread_cond_init(&thread_confucius, NULL);
    pthread_cond_init(&thread_epicurus, NULL);
    
    // Create consumer and producer thread.
    pthread_create(&tidPlato, NULL, plato, NULL);
    printf("PLato thread created.\n");
    pthread_create(&tidAristotle, NULL, aristotle, NULL);
    printf("Aristotle thread created.\n");
    pthread_create(&tidSocrates, NULL, socrates, NULL);
    printf("Socrates thread created.\n");
    pthread_create(&tidConfucius, NULL, confucius, NULL);
    printf("Confucius thread created.\n");
    pthread_create(&tidEpicurus, NULL, epicurus, NULL);
    printf("Epicurus thread created.\n");
    
    // When done join threads.
    pthread_join(tidPlato, NULL);
    pthread_join(tidAristotle, NULL);
    pthread_join(tidSocrates, NULL);
    pthread_join(tidConfucius, NULL);
    pthread_join(tidEpicurus, NULL);
    
    //Destroy pthreads so they don't continue after ctrl c
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&thread_plato);
    pthread_cond_destroy(&thread_aristotle);
    pthread_cond_destroy(&thread_socrates);
    pthread_cond_destroy(&thread_confucius);
    pthread_cond_destroy(&thread_epicurus);
    printf("Threads are completed.\n");
    
    exit(0);
}

