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
void print_help(Status resource[maxSize], int num) {
    int i;
    int empty = 0;
    for (i = 0; i < maxSize; i++) {
        if (resource[i].count == 1) {
            printf("Resource %d is currently being used by process %s\n",num,resource[i].status);
            empty = 1;
        }
    }
    if (empty == 0) {
        printf("Resource %d is currently not being used by any processes\n",num);
    }
    printf("***************************************\n");
}
//Print current results
void print_results() {
    printf("Process Status\n");
    print_help(resource1, 1);
    print_help(resource2, 2);
    print_help(resource3, 3);
}
void get_resource(int n, pthread_cond_t phil, int name) {
    //Change philospher status
    Status getResource;
    //0 for false since you don't have the fork yet
    if (name == 1) {getResource = (Status){.status = "one",.count = 1};}
    if (name == 2) {getResource = (Status){.status = "two",.count = 1};}
    if (name == 3) {getResource = (Status){.status = "three",.count = 1};}
    if (name == 4) {getResource = (Status){.status = "four",.count = 1};}
    pthread_mutex_lock(&thread_mutex);
    //Find an open resource
    if(resource1[0].count == 0 || resource1[1].count == 0 || resource1[2].count == 0) {
        if (resource1[0].count == 0) {
            resource1[0] = getResource;
        } else if (resource1[1].count == 0) {
            resource1[1] = getResource;
        } else {
            resource1[2] = getResource;
        }
    } else if (resource2[0].count == 0 || resource2[1].count == 0 || resource2[2].count == 0) {
        if (resource2[0].count == 0) {
            resource2[0] = getResource;
        } else if (resource2[1].count == 0) {
            resource2[1] = getResource;
        } else {
            resource2[2] = getResource;
        }
    } else if (resource3[0].count == 0 || resource3[1].count == 0 || resource3[2].count == 0) {
        if (resource3[0].count == 0) {
            resource3[0] = getResource;
        } else if (resource3[1].count == 0) {
            resource3[1] = getResource;
        } else {
            resource3[2] = getResource;
        }
    } else {
        //This code should never occur
        printf("Error all resources full!\n");
    }
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
}
void isEmptied(Status resource[maxSize]) {
    //Write code
    printf("Releasing resource\n");
}
//Release status set count to 2. When all counts are 2 fully release resource
void release_resource(int n, pthread_cond_t phil, int name) {
    //Change philospher status
    Status relResource;
    //1 for true since you still have the fork
    if (name == 1) {relResource = (Status){.status = "",.count = 2};}
    if (name == 2) {relResource = (Status){.status = "",.count = 2};}
    if (name == 3) {relResource = (Status){.status = "",.count = 2};}
    if (name == 4) {relResource = (Status){.status = "",.count = 2};}
    pthread_mutex_lock(&thread_mutex);
    int i;
    if (name == 1) {
        for (i = 0; i < maxSize; i++) {
            if (strcmp(resource1[i].status,"one") == 0) {
                resource1[i] = relResource;
                isEmptied(resource1);
            } else if (strcmp(resource2[i].status,"one") == 0) {
                resource2[i] = relResource;
                isEmptied(resource2);
            } else if (strcmp(resource3[i].status,"one") == 0) {
                resource3[i] = relResource;
                isEmptied(resource3);
            } else {
                printf("Process one not currently active\n");
            }
        }
    }
    if (name == 2) {
        for (i = 0; i < maxSize; i++) {
            if (strcmp(resource1[i].status,"two") == 0) {
                resource1[i] = relResource;
                isEmptied(resource1);
            } else if (strcmp(resource2[i].status,"two") == 0) {
                resource2[i] = relResource;
                isEmptied(resource2);
            } else if (strcmp(resource3[i].status,"two") == 0) {
                resource3[i] = relResource;
                isEmptied(resource3);
            } else {
                printf("Process two not currently active\n");
            }
        }
    }
    if (name == 3) {
        for (i = 0; i < maxSize; i++) {
            if (strcmp(resource1[i].status,"three") == 0) {
                resource1[i] = relResource;
                isEmptied(resource1);
            } else if (strcmp(resource2[i].status,"three") == 0) {
                resource2[i] = relResource;
                isEmptied(resource2);
            } else if (strcmp(resource3[i].status,"three") == 0) {
                resource3[i] = relResource;
                isEmptied(resource3);
            } else {
                printf("Process three not currently active\n");
            }
        }
    }
    if (name == 4) {
        for (i = 0; i < maxSize; i++) {
            if (strcmp(resource1[i].status,"four") == 0) {
                resource1[i] = relResource;
                isEmptied(resource1);
            } else if (strcmp(resource2[i].status,"four") == 0) {
                resource2[i] = relResource;
                isEmptied(resource2);
            } else if (strcmp(resource3[i].status,"four") == 0) {
                resource3[i] = relResource;
                isEmptied(resource3);
            } else {
                printf("Process four not currently active\n");
            }
        }
    }
    //Print new status
    print_results();
    pthread_mutex_unlock(&thread_mutex);
    pthread_cond_signal(&phil);
    //Output new results
    print_results();
}
//Create first process
void *one(void* ptr)
{
    while(1) {
        find();
        get_resource(0,thread_one,1);
        use();
        release_resource(0,thread_one,1);
    }
}
//Create second process
void *two(void* ptr)
{
    while(1) {
        find();
        get_resource(1,thread_two,2);
        use();
        release_resource(1,thread_two,2);
    }
}
//Create third process
void *three(void* ptr)
{
    while(1) {
        find();
        get_resource(2,thread_three,3);
        use();
        release_resource(2,thread_three,3);
    }
}
//Create fourth process
void *four(void* ptr)
{
    while(1) {
        find();
        get_resource(3,thread_four,4);
        use();
        release_resource(3,thread_four,4);
    }
}
void initResource(){
    int i;
    Status def;
    for (i = 0; i < maxSize; i++){
        def = (Status){.status = "",.count = 0};
        resource1[i] = def;
        resource2[i] = def;
        resource3[i] = def;
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
    //Create resources
    initResource();
    
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

