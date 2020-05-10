#include "header.h"


client_queue * queue_initialize(int max_size)
{
    client_queue * queue = (client_queue *) calloc(1, sizeof(client_queue));
    queue->max_len = max_size;
    queue->curr_len = 0;
    queue->first=NULL;
    queue->last=NULL;

    return queue;
}

int queue_is_full(client_queue * q)
{
    if (q->max_len == q->curr_len)
    {
        return TRUE;
    }
    return FALSE;
}

int queue_is_empty(client_queue * q)
{
    if (q->curr_len == 0)
    {
        return TRUE;
    }
    return FALSE;
}

void add_client(client_queue * queue, pthread_t client_id)
{
    node * client = (node *) calloc(1, sizeof(node));
    client->client_id = client_id;
    client->next = NULL;

    if (queue->last == NULL)
    {
        queue->last = client;
        queue->first = client;
        queue->curr_len++;    
    }
    else
    {
        queue->last->next = client;
        queue->last = client;
        queue->curr_len++;
    }
    
}

node * shave_client(client_queue * queue)
{
    node * to_shave = queue->first;

    if(queue->curr_len == 1)
    {
        queue->first= NULL;
        queue->last= NULL;
    }
    else
    {
        queue->first = queue->first->next;
    }
    queue->curr_len--;      

    return to_shave;

}