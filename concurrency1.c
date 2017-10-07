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

//For building structs information used from https://stackoverflow.com/questions/32698293/assign-values-to-structure-variables
typedef struct data {
    int number;
    int time;
} Data;

Data arrBuffer[32];
int count = 0;

void* consumer(void *ptr) {
    int wait = 0;
    unsigned int timeWait = 0;
    while(1) {
        if (count <= 0) {
            printf("Buffer Empty: Waiting on producer...\n");
            count = 0;
            wait = 1;
        }
        //Consume data from array data structure
        printf("Item Value: %d\n",arrBuffer[count].number);
        timeWait = arrBuffer[count].time;
        //Wait for specific amount of time
        unsigned int time_to_sleep = timeWait;
        while(time_to_sleep)
            time_to_sleep = sleep(time_to_sleep);
        count--;
        
    }
}
void* producer(void *ptr) {
    int wait = 0;
    while(1) {
        //If count is higher than Max size then wait for consumer to remove data
        if (count >= maxSize) {
            printf("Buffer Full: Waiting on consumer...\n");
            count = 32;
            wait = 1;
        }
        //Build new data
        Data p1;
        //Number 1-100, Time between 2 and 9. 3,4,5,6,7,8
        p1 = (Data){.number = (genrand_int32() % 99) + 1, .time = (genrand_int32() % 6) + 3};
        arrBuffer[count] = p1;
        count++;
    }
}

int main(int argc, char **argv)
{
	// Thread ID.
	pthread_t tidConsumer;    
	pthread_t tidProducer;    
	int result_code;
	
	// Create consumer and producer thread.
	result_code = pthread_create(&tidConsumer, NULL, consumer, NULL);
	printf("Consumer thread created.\n");
	result_code = pthread_create(&tidProducer, NULL, producer, NULL);
	printf("Producer thread created.\n");

	// When done.
	result_code = pthread_join(tidProducer, NULL);
	//assert(0 == result_code);
	result_code = pthread_join(tidConsumer, NULL);
	//assert(0 == result_code);
	printf("Threads are completed.\n");

	exit(0);
}
