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
//For bool
#include <stdbool.h>
#define maxSize 3 //Table will hold 4 processes

typedef struct status {
    char status[15];
    int count;
} Status;

//Three resources the process will work on
Status resource1[maxSize];
Status resource2[maxSize];
Status resource3[maxSize];

char names [4][15] = {"one", "two","three","four"};

pthread_cond_t thread_one, thread_two, thread_three, thread_four;
pthread_mutex_t thread_mutex;

//Find a resource
void find() {
    //Find for 1-10 seconds
    unsigned int find_sleep = (genrand_int32() % 9) + 1;
    while(find_sleep)
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
void print_help (Status resource[maxSize], int num) {
    int i;
    int empty = 0;
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
    getResource = (Status){.status = name,.count = 1};
    pthread_mutex_lock(&thread_mutex);
    //Find an open resource
    if(isAvailable(resource1)) {
        resource1[0] = getResource;
    } else if (isAvailable(resource2)) {
        resource2[0] = getResource;
    } else if (isAvailable(resource3)) {
        resource3[0] = getResource;
    } else {
        //This code should never occur
        printf("Error all resources full!");
    }
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
}
//Find available resource
bool isAvailable(Status resource[maxSize]) {
    //Set counts to 1. When release resource just release status but keep count
    if (resource[0].count != 0 && resource[1].count != 0 && resource[2].count != 0) {
        //Return 0 for false
        return 0;
    }
    //Return 1 for true
    return 1;
}
//Release status set count to 2. When all counts are 2 fully release resource
void release_resource(int n, pthread_cond_t phil, char name [15]) {
    //Change philospher status
    Status relResource;
    //1 for true since you still have the fork
    relResource = (Status){.status = "",.count = 2};
    pthread_mutex_lock(&thread_mutex);
    int i;
    for (i = 0; i < maxSize; i++) {
        if (resource1[i].status == name) {
            resource1[i] = relResource;
            isEmptied(resource1);
        } else if (resource2[i].status == name) {
            resource2[i] = relResource;
            isEmptied(resource2);
        } else if (resource3[i].status == name) {
            resource3[i] = relResource;
            isEmptied(resource3);
        } else {
            printf("Error process not found!");
        }
    }
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
    pthread_cond_signal(&phil);
    //Output new results
    print_results();
}
void isEmptied(Status resource[maxSize]) {
    //Write code
    printf("Releasing resource");
}
//Create first process
void *one(void* ptr)
{
    while(1) {
        find();
        get_resource(0,thread_one,names[1]);
        use();
        release_resource(0,thread_one,names[1]);
    }
}
//Create second process
void *two(void* ptr)
{
    while(1) {
        find();
        get_resource(1,thread_two,names[2]);
        use();
        release_resource(1,thread_two,names[2]);
    }
}
//Create third process
void *three(void* ptr)
{
    while(1) {
        find();
        get_resource(2,thread_three,names[3]);
        use();
        release_resource(2,thread_three,names[3]);
    }
}
//Create fourth process
void *four(void* ptr)
{
    while(1) {
        find();
        get_resource(3,thread_four,names[4]);
        use();
        release_resource(3,thread_four,names[4]);
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

