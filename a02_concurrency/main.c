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

typedef struct status {
    int fork;
    char status[15];
} Status;

Status arrPhilosophers[maxSize];

char names [maxSize][15] = {"plato", "aristotle","socrates","confucius","epicurus"};

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
//Print current results
void print_results() {
    printf("Fork Status\n");
    int i;
    for (i = 0; i < maxSize; i++) {
        if (arrPhilosophers[i].fork == 1) {
            printf("Fork %d is currently held by %s\n",i+1,names[i]);
        } else {
            printf("Fork %d is currently not being held\n",i+1);
        }
    }
    printf("Philosopher Status\n");
    int j;
    for (j = 0; j < maxSize; j++) {
        printf("%s is currently %s\n",names[j],arrPhilosophers[j+1].status);
    }
    printf("***************************************\n");
}
void get_forks(int n, pthread_cond_t phil) {
    //Change philospher status
    Status getFork;
    //0 for false since you don't have the fork yet
    getFork = (Status){.status = "Getting Fork",.fork = 0};
    pthread_mutex_lock(&thread_mutex);
    arrPhilosophers[n] = getFork;
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
    //Change Status
    getFork = (Status){.status = "Eating",.fork = 1};
    arrPhilosophers[n] = getFork;
    //Lock Mutex for thread use
    pthread_mutex_lock(&thread_mutex);
    pthread_cond_signal(&phil);
    ////Unlock mutex so other threads can use data structure
    pthread_mutex_unlock(&thread_mutex);
    //Output new results
    print_results();
}
void put_forks(int n, pthread_cond_t phil) {
    //Change philospher status
    Status putFork;
    //1 for true since you still have the fork
    putFork = (Status){.status = "Putting Fork",.fork = 1};
    pthread_mutex_lock(&thread_mutex);
    arrPhilosophers[n] = putFork;
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
    //Change Status
    putFork = (Status){.status = "Thinking",.fork = 0};
    arrPhilosophers[n] = putFork;
    //Lock Mutex for thread use
    pthread_mutex_lock(&thread_mutex);
    pthread_cond_signal(&phil);
    ////Unlock mutex so other threads can use data structure
    pthread_mutex_unlock(&thread_mutex);
    //Output new results
    print_results();
}

void *plato(void* ptr)
{
    while(1) {
        think();
        get_forks(1,thread_plato);
        eat();
        put_forks(1,thread_plato);
    }
}

void *aristotle(void* ptr)
{
    while(1) {
        think();
        get_forks(2,thread_aristotle);
        eat();
        put_forks(2,thread_aristotle);
    }
}

void *socrates(void* ptr)
{
    while(1) {
        think();
        get_forks(3,thread_socrates);
        eat();
        put_forks(3,thread_socrates);
    }
}

void *confucius(void* ptr)
{
    while(1) {
        think();
        get_forks(4,thread_confucius);
        eat();
        put_forks(4,thread_confucius);
    }
}

void *epicurus(void* ptr)
{
    while(1) {
        think();
        get_forks(5,thread_epicurus);
        eat();
        put_forks(5,thread_epicurus);
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
    printf("***************************************\n");
    
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

