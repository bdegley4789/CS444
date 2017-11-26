//Bryce Egley and Bruce Garcia
//CS 444
//Concurrency exercise 3: Part 2
//We re-used some of the code from concurrency exercise #1 and #2
// Source: http://www.zentut.com/c-tutorial/c-linked-list/
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

typedef enum{ON,OFF} status_t;

char names [maxSize][15] = {"searcher", "inserter","deleter"};
//Linked List of int's
typedef struct node {
    int data;
    struct node* next;
} node;
int count;
node *head;
status_t state_deleter = OFF;
status_t state_inserter = OFF;

pthread_cond_t thread_searcher, thread_inserter, thread_deleter;
pthread_mutex_t thread_mutex;

/*void think() {
    //Think for 1-5 seconds
    unsigned int think_sleep = (genrand_int32() % 4) + 1;
    while(think_sleep)
        think_sleep = sleep(think_sleep);
}*/
void execute() {
    //Eat for 1-5 seconds
    unsigned int eat_sleep = (genrand_int32() % 4) + 1;
    while(eat_sleep)
        eat_sleep = sleep(eat_sleep);
}
//Insert new element to end of linked list
/*
 create a new node
 initialize the data and next field
 
 return the newly created node
 */
node* create(int data,node* next)
{
    node* new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL)
    {
        printf("Error creating a new node.\n");
        exit(0);
    }
    new_node->data = data;
    new_node->next = next;
    
    return new_node;
}
/*
 add a new node at the beginning of the list
 */
node* prepend(node* theHead,int data)
{
    node* new_node = create(data,theHead);
    theHead = new_node;
    return theHead;
}
//Add new data to end of linked list
node* append(node* head, int data)
{
    if(head == NULL)
        return NULL;
    /* go to the last node */
    node *cursor = head;
    while(cursor->next != NULL)
        cursor = cursor->next;
    
    /* create a new node */
    node* new_node =  create(data,NULL);
    cursor->next = new_node;
    
    return head;
}
int find(int position)
{
    if(head==NULL)
        return 0;
    /* Go to position */
    node *cursor = head;
    int i;
    for(i = 0; i < position; i++) {
        cursor = cursor->next;
    }
    /* Get Data */
    return cursor->data;
}

// Create a link list of five elements
node* create_struct(node* theHead) {
    
    int iterations = 5;
    int i;
    init_genrand(time(NULL));
    
    // Generate a number between 1-20
    int number = (genrand_int32() % 20) + 1;
    theHead = prepend(theHead, number);
    
    for(i = 0; i < iterations; i++) {
        // Generate a number between 1-20
        number = (genrand_int32() % 20) + 1;
        theHead =  append(theHead, number);
        printf("Current value %d\n", theHead->data);
        count++;
    }
    return theHead;
}
void delete(int position)
{
    if(head == NULL)
        printf("Empty linked list");
    /* Go to position */
    node *cursor = head;
    int i;
    //Stop one before
    for(i = 0; i < position-1; i++) {
        cursor = cursor->next;
    }
    /* delete node */
    cursor->next = cursor->next->next;
}
//Add new data to end of linked list
void append2(int data)
{
    if(head == NULL)
        printf("Empty linked list");
    /* go to the last node */
    node *cursor = head;
    while(cursor->next != NULL)
        cursor = cursor->next;
    
    /* create a new node */
    node* new_node =  create(data,NULL);
    cursor->next = new_node;
}
int search() {
    execute();
    unsigned int searchPos = genrand_int32() % count;
    return find(searchPos);
}
int insert_item() {
    //Random data 0-99999
    execute();
    unsigned int data = genrand_int32() % 100000;
    //Add new data to end of node
    append2(data);
    return data;
}
int delete_item() {
    //Position to delete
    execute();
    unsigned int deletePos = genrand_int32() % count;
    //Find data about to be deleted
    int data = find(deletePos);
    //Delete node at specific position
    delete(deletePos);
    //Decrement count
    count--;
    return data;
}
// Searchers
// They merely examine the list; hence thye can execute concurrently w/ each other

/*void *searchers(void* ptr)
 {
 
 node * head;
 head = (node *) ptr;
 while(1) {
 // searching the linked list for data
 printf("Searching...\n");
 int data = (genrand_int32() % 20) + 1;
 node *temp = search(head, data);
 printf("Searching...\n");
 if (temp != NULL)
 {
 printf("temp value = %d\n", temp->data);
 }
 }
 }*/


// Searcher
// They merely examine the list; hence thye can execute concurrently w/ each other

void *searcher(void* ptr)
{
    while(1) {
        /*while(state_deleter == ON) {
            printf("Waiting for deleter to finish");
        }*/
        //think();
        //pthread_mutex_lock(&thread_mutex);
        printf("Searcher...\n");
        if (count <= 0) {
            printf("LinkedList is empty. Waiting for inserter");
            //pthread_mutex_unlock(&thread_mutex);

        } else {
            printf("Item %d found\n",search());
            //pthread_mutex_unlock(&thread_mutex);
        }
        //execute();
    }
}

// Inserters
// They add new items to the end of the list; they must be mutually exclusive to preclude
// two inserters from inserting new items at about the same time.
void *inserter(void* ptr)
{
    while(1) {
        /*while(state_deleter == ON) {
            printf("Waiting for deleter to finish");
        }
        while(state_inserter == ON) {
            printf("Waiting for inserter to finish");
        }*/
        //think();
        printf("Inserter...\n");
        state_inserter = ON;
        pthread_mutex_lock(&thread_mutex);
        printf("Item %d inserted. Count %d\n",insert_item(),count);
        count++;
        pthread_cond_signal(&thread_inserter);
        pthread_mutex_unlock(&thread_mutex);
        //execute();
        //Change state
        state_inserter = OFF;
        sleep(1);
    }
}

// Deleters
// They remove items from anywhre in the list. At most one deleter process can access
// the list at a time, and deletion must also be mutually exclusive with searches and insertions.
void *deleter(void* ptr)
{
    while(1) {
        /*while(state_deleter == ON) {
            printf("Waiting for deleter to finish");
        }
        while(state_inserter == ON) {
            printf("Waiting for inserter to finish");
        }*/
        //think();
        printf("Deleter...\n");
        sleep(4);
        state_deleter = ON;
        pthread_mutex_lock(&thread_mutex);
        if (count <= 0) {
            printf("LinkedList is empty. Waiting for inserter");
            pthread_mutex_unlock(&thread_mutex);
        } else {
            pthread_mutex_unlock(&thread_mutex);
            pthread_mutex_lock(&thread_mutex);
            printf("Item %d deleted\n",delete_item());
            //Unlock mutex for shared resource
            pthread_cond_signal(&thread_deleter);
            pthread_mutex_unlock(&thread_mutex);
            //execute();
            //Change State
            state_deleter = OFF;
        }
    }
}

int main() {

    //Have seed generate for random sequnce of numbers with genrand
    init_genrand(time(NULL));
    head = NULL;
    head = create_struct(head);

    // Thread ID.
    pthread_t tidDeleter;
    pthread_t tidSearcher;
    pthread_t tidInserter;
    
    //Create mutex so threads can both use shared resource
    if(pthread_mutex_init(&thread_mutex, NULL)) {
        fprintf(stderr, "Error creating mutex");
        return 1;
    }
    
    //Initialize conditional vars so thread can wait until condition occurs
    pthread_cond_init(&thread_inserter, NULL);
    pthread_cond_init(&thread_deleter, NULL);
    pthread_cond_init(&thread_searcher, NULL);
    
    // Create consumer and producer thread.
    pthread_create(&tidInserter, NULL, inserter, NULL);
    printf("Inserter thread created.\n");
    pthread_create(&tidDeleter, NULL, deleter, NULL);
    printf("Deleter thread created.\n");
    pthread_create(&tidSearcher, NULL, searcher, NULL);
    printf("Searcher thread created.\n");
    
    // When done join threads.
    pthread_join(tidInserter, NULL);
    pthread_join(tidDeleter, NULL);
    pthread_join(tidSearcher, NULL);
    
    //Destroy pthreads so they don't continue after ctrl c
    pthread_mutex_destroy(&thread_mutex);
    pthread_cond_destroy(&thread_inserter);
    pthread_cond_destroy(&thread_deleter);
    pthread_cond_destroy(&thread_searcher);
    printf("Threads are completed.\n");
    
    exit(0);
}

