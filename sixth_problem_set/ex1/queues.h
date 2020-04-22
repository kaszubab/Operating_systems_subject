#ifndef queues

#include "sys/msg.h"
#include "sys/ipc.h"

#define queues

#define SEED 227
#define MESSAGE_LEN 256
#define MAXIMUM_CLIENTS 20

#define TRUE 1
#define FALSE 0

typedef struct task
{
    long type;
    int client_id;
    key_t queue_id;
    pid_t client_pid;
    char task_data[MESSAGE_LEN];
} task;

typedef enum type {
    STOP = 1, DISCONNECT = 2, LIST = 3, CONNECT = 4, INIT =5 
} type;

const size_t MESSAGE_SIZE = sizeof(task) - sizeof(long);





#endif