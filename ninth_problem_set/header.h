
#ifndef header
#define header

#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"


#define TRUE 1
#define FALSE 0

typedef struct node
{
    pthread_t client_id;
    struct node * next;
} node;

typedef struct client_queue 
{
    node * first;
    node * last;
    int curr_len;
    int max_len;
} client_queue;

client_queue * queue_initialize(int max_size);
int queue_is_full(client_queue * q);
int queue_is_empty(client_queue * q);
void add_client(client_queue * queue, pthread_t client_id);
node * shave_client(client_queue * queue);




#endif