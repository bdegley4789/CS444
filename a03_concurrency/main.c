//Bryce Egley and Bruce Garcia
//CS 444
//Concurrency exercise 3: Part 1
//We re-used some of the code from concurrency exercises #1 and #2
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.c"
#include <assert.h>
#define maxSize 3 //Table will hold 4 processes

typedef struct status {
    char status[15];
    int count;
} Status;

//Three resources the process will work on
Status resource1[3];
Status resource2[3];
Status resource3[3];

char names [4][15] = {"one", "two","three","four"};

pthread_cond_t thread_one, thread_two, thread_three, thread_four;
pthread_mutex_t thread_mutex;

//Find a resource
void find() {
    //Find for 1-10 seconds
    unsigned int find_sleep = (genrand_int32() % 9) + 1;
    while(think_find)
        find_sleep = sleep(find_sleep);
}
//Use a resource
void use() {
    //Use for 1-20 seconds
    unsigned int use_sleep = (genrand_int32() % 19) + 1;
    while(use_sleep)
        use_sleep = sleep(use_sleep);
}
//Print current results
void print_results() {
    printf("Process Status\n");
    print_help(resource1, 1);
    print_help(resource2, 2);
    print_help(resource3, 3);
}
void print_help (Status resource, int num) {
    int i;
    int empty = 0
    for (i = 0; i < maxSize; i++) {
        if (resource[i].count == 1) {
            printf("Resource %d is currently being used by process %s\n",num,resource[i].status);
            empty = 1;
        }
    }
    if (empty == 0) {
        printf("Resource %d is currently not being used by any resource\n",num);
    }
    printf("***************************************\n");
}
void get_resource(int n, pthread_cond_t phil, char name [15]) {
    //Change philospher status
    Status getResource;
    //0 for false since you don't have the fork yet
    getResource = (Status){.status = name,.count = 0};
    pthread_mutex_lock(&thread_mutex);
    //Find an open resource
    find();
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
void find() {
    
}
void release_resource(int n, pthread_cond_t phil) {
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

void *one(void* ptr)
{
    while(1) {
        find();
        get_resource(0,thread_one);
        use();
        release_resource(0,thread_one);
    }
}

void *two(void* ptr)
{
    while(1) {
        find();
        get_resource(1,thread_two);
        use();
        release_resource(1,thread_two);
    }
}

void *three(void* ptr)
{
    while(1) {
        find();
        get_resource(2,thread_three);
        use();
        release_resource(2,thread_three);
    }
}

void *four(void* ptr)
{
    while(1) {
        find();
        get_resource(3,thread_four);
        use();
        release_resource(3,thread_four);
    }
}

int main() {
    //Have seed generate for random sequnce of numbers with genrand
    init_genrand(time(NULL));
    // Thread ID.
    pthread_t pid_one;
    pthread_t pid_two;
    pthread_t pid_three;
    pthread_t pid_four;
    
    //Create mutex so threads can both use shared resource
    if(pthread_mutex_init(&thread_mutex, NULL)) {
        fprintf(stderr, "Error creating mutex");
        return 1;
    }
    
    //Initialize conditional vars so thread can wait until condition occurs
    pthread_cond_init(&thread_one, NULL);
    pthread_cond_init(&thread_two, NULL);
    pthread_cond_init(&thread_three, NULL);
    pthread_cond_init(&thread_four, NULL);
    
    // Create consumer and producer thread.
    pthread_create(&pid_one, NULL, one, NULL);
    printf("Thread 1 created.\n");
    pthread_create(&pid_two, NULL, two, NULL);
    printf("Thread 2 created.\n");
    pthread_create(&pid_three, NULL, three, NULL);
    printf("Thread 3 created.\n");
    pthread_create(&pid_four, NULL, four, NULL);
    printf("Thread 4 created.\n");
    printf("***************************************\n");
    
    // When done join threads.
    pthread_join(pid_one, NULL);
    pthread_join(pid_two, NULL);
    pthread_join(pid_three, NULL);
    pthread_join(pid_four, NULL);
    
    //Destroy pthreads so they don't continue after ctrl c
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&thread_one);
    pthread_cond_destroy(&thread_two);
    pthread_cond_destroy(&thread_three);
    pthread_cond_destroy(&thread_four);
    printf("Threads are completed.\n");
    
    exit(0);
}

