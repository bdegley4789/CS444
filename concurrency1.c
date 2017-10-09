//Bryce Egley and Bruce Garcia
//CS 444
//Concurrency exercise 1: Producer-Consumer Problem
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.c"
#include <assert.h>
#define maxSize 32 //Buffer can hold 32 items

//Information on building structs used and modified from the following
//https://stackoverflow.com/questions/32698293/assign-values-to-structure-variables
typedef struct data {
    int number;
    int time;
} Data;

pthread_cond_t thread_consumer, thread_producer;
pthread_mutex_t thread_mutex;

//Array data structure of 32 size
Data arrBuffer[maxSize];
//Count to keep track of size
int count = 0;

//Consumer thread
void* consumer(void *ptr) {
    while(1) {
        //Lock to use count the shared resource
        pthread_mutex_lock(&thread_mutex);
        while (count <= 0) {
            printf("Buffer Empty: Waiting on producer...\n");
            pthread_cond_wait(&thread_consumer, &thread_mutex);
        }
        pthread_mutex_unlock(&thread_mutex);
        //Consume data from array data structure
        //printf("Item Value: %d\n",arrBuffer[count-1].number);
        printf("ITEM CONSUMED Value: %d Time: %d Count %d\n",arrBuffer[count-1].number,arrBuffer[count-1].time,count);
        unsigned int timeWait = arrBuffer[count-1].time;
        //Wait for specific amount of time
        unsigned int time_to_sleep = timeWait;
        while(time_to_sleep)
            time_to_sleep = sleep(time_to_sleep);
        //Lock to use count the shared resource
        pthread_mutex_lock(&thread_mutex);
        count--;
        pthread_cond_signal(&thread_producer);
        pthread_mutex_unlock(&thread_mutex);
    }
}

//Producer thread
void* producer(void *ptr) {
    while(1) {
        pthread_mutex_lock(&thread_mutex);
        //If count is higher than Max size then wait for consumer to remove data
        while (count >= maxSize) {
            printf("Buffer Full: Waiting on consumer...\n");
            pthread_cond_wait(&thread_producer, &thread_mutex);
        }
        pthread_mutex_unlock(&thread_mutex);
        //Build new data
        Data p1;
        //Number 1-100, Time between 2 and 9. 3,4,5,6,7,8
        p1 = (Data){.number = (genrand_int32() % 99) + 1, .time = (genrand_int32() % 6) + 3};
        //Producer wait 3-7 seconds
        unsigned int producer_sleep = (genrand_int32() % 5) + 3;
        //To use shared resource(count and arrBuffer) put mutex lock on
        pthread_mutex_lock(&thread_mutex);
        printf("ITEM PRODUCED Value: %d Time: %d Wait: %d Count: %d\n",p1.number,p1.time,producer_sleep,count);
        arrBuffer[count] = p1;
        count++;
        pthread_cond_signal(&thread_consumer);
        pthread_mutex_unlock(&thread_mutex);
        //Call sleep after mutex is unlock so consumer thread can run during this time
        while(producer_sleep)
            producer_sleep = sleep(producer_sleep);
    }
}
//Information on pthreads used and modified from the following
//http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
//http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_join.html
//Information on mutex lock threads used and modified from the following
//https://stackoverflow.com/questions/14888027/mutex-lock-threads
//http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_cond_init.html
int main()
{
    //Have seed generate for random sequnce of numbers with genrand
    init_genrand(time(NULL));
	// Thread ID.
	pthread_t tidConsumer;    
	pthread_t tidProducer;
	int result_code;
    
    //Create mutex so threads can both use shared resource
    if(pthread_mutex_init(&thread_mutex, NULL)) {
        fprintf(stderr, "Error creating mutex");
        return 1;
    }
    //Initialize conditional variables so thread can wait until condition occurs
    if(pthread_cond_init(&thread_producer, NULL)) {
        fprintf(stderr, "Error creating producer");
        return 1;
    }
    if(pthread_cond_init(&thread_consumer, NULL)) {
        fprintf(stderr, "Error creating consumer");
        return 1;
    }
	// Create consumer and producer thread.
    if(pthread_create(&tidProducer, NULL, producer, NULL)) {
        fprintf(stderr, "Error creating producer thread\n");
        return 1;
    }
    printf("Producer thread created.\n");
    if(pthread_create(&tidConsumer, NULL, consumer, NULL)) {
        fprintf(stderr, "Error creating consumer thread\n");
        return 1;
    }
	printf("Consumer thread created.\n");
	// When done join threads.
    if(pthread_join(tidProducer, NULL)) {
        fprintf(stderr, "Error joining producer thread\n");
        return 2;
    }
    if(pthread_join(tidConsumer, NULL)) {
        fprintf(stderr, "Error joining consumer thread\n");
        return 2;
    }
    //Destroy pthreads so they don't continue after ctrl c
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&thread_producer);
    pthread_cond_destroy(&thread_consumer);
	printf("Threads are completed.\n");

	exit(0);
}
