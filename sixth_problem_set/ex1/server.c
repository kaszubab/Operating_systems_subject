#include "stdio.h"
#include "stdlib.h"
#include "signal.h"

#include "queues.h"

int server_queue = -1;
int client_queues[MAXIMUM_CLIENTS];
int client_keys[MAXIMUM_CLIENTS];
int client_ids[MAXIMUM_CLIENTS];
int client_pids[MAXIMUM_CLIENTS];
int free_now[MAXIMUM_CLIENTS];
int current_ID = 1;

void init_client(key_t queue_id, pid_t client_pid) 
{  
    int i = 0;
    for (; i < MAXIMUM_CLIENTS; i++)
    {
        if (client_queues[i] == -1)
        {
            break;
        }
    }

    if ( i == MAXIMUM_CLIENTS)
    {
        printf("Client list full");
        return;
    }

    fflush(stdout);
    

    int client_queue = msgget(queue_id, IPC_CREAT);

    if (client_queue == -1)
    {
        printf("Cant open client queue");
        return;
    }

    client_queues[i] = client_queue;
    client_keys[i] = queue_id;
    client_ids[i] = current_ID;
    client_pids[i] = client_pid;
    current_ID++; 
    free_now[i] = TRUE;

    task new_task;
    new_task.type = INIT;
    new_task.client_id = client_ids[i];

    if (msgsnd(client_queues[i], &new_task, MESSAGE_SIZE, 0) != 0)
    {
        printf("Cant send init response");
        return;
    }

}

void stop(int client_id)
{
    int i = 0;
    for (; i < MAXIMUM_CLIENTS; i++)
    {
        if (client_ids[i] == client_id)
        {
            break;
        }
    }

    client_ids[i] = -1;
    client_queues[i] = -1;
    free_now[i] = FALSE;
}

void disconnect(int client_id)
{
    int i = 0;
    for (; i < MAXIMUM_CLIENTS; i++)
    {
        if (client_ids[i] == client_id)
        {
            break;
        }
    }

    free_now[i] = TRUE;
}

void connect(int caller1, int caller2)
{
    int client1 = 0, client2 = 0;

    for (int i = 0; i < MAXIMUM_CLIENTS; i++)
    {
        if ( client_ids[i] == caller1)
        {
            client1 = i;
        }
        if ( client_ids[i] == caller2)
        {
            client2 = i;
        }
    }

    if (free_now[client1] == FALSE || free_now[client2] == FALSE)
    {
        printf("Either of the clients is busy\n");
        return;
    }

    task new_task;
    new_task.queue_id = client_queues[client2];
    new_task.type = CONNECT;

    if (msgsnd(client_queues[client1], &new_task, MESSAGE_SIZE, 0) != 0)
    {
        printf("Cant send the message\n");
        return;
    }

    new_task.queue_id = client_queues[client1];
    new_task.type = CONNECT;

    if (msgsnd(client_queues[client2], &new_task, MESSAGE_SIZE, 0) != 0)
    {
        printf("Cant send the message\n");
        return;
    }

    kill(client_pids[client2], SIGUSR1);
    free_now[client1] = FALSE;
    free_now[client2] = FALSE;
    return;

}

void process_task(task * curr_task)
{
    printf("Processin! ");
    fflush(stdout);
    switch (curr_task->type)
    {
    case INIT:
        printf("Logging new client %d \n", curr_task->queue_id);
        fflush(stdout);
        init_client(curr_task->queue_id, curr_task->client_pid);
        break;

    case STOP:
        printf("Removing %d from clients \n", curr_task->client_id);
        stop(curr_task->client_id);
        break;

    case DISCONNECT:
        printf("Disconnecting %d \n", curr_task->client_id);
        disconnect(curr_task->client_id);
        break;

    case CONNECT:
        printf("Connecting %d and %s \n", curr_task->client_id, curr_task->task_data );
        connect(curr_task->client_id, atoi(curr_task->task_data));
        break;

    case LIST:
        printf("List of available clients \n");
        for (int i = 0 ; i < MAXIMUM_CLIENTS; i++)
        {
            if (client_ids[i] >= 1)
            {
                printf("Client id %d available %d \n", client_ids[i], free_now[i]);
            }
        }
        printf("--------------------------\n");
        break;

    
    default:
        break;
    }
}

void sigint_handler(int sig)
{
    printf("Exiting server and closing all open queues. Goodbye! \n");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

void shutdown_server()
{
    for (int i = 0; i < MAXIMUM_CLIENTS; i++)
    {
        if (client_ids[i] > 0)
        {
            task new_task;
            new_task.type = STOP;

            if (msgsnd(client_queues[i], &new_task, MESSAGE_SIZE, 0) != 0)
            {
                perror("Can't send stop message"); 
                msgctl(server_queue, IPC_RMID, NULL);
                exit(EXIT_FAILURE);
            }
            

            kill(client_pids[i], SIGUSR1);

            if (msgrcv(server_queue, &new_task, MESSAGE_SIZE, STOP, 0) < 0)
            {
                perror("Shuting down, can't receive message");
                msgctl(server_queue, IPC_RMID, NULL);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (msgctl(server_queue, IPC_RMID, NULL))
    {
        perror("Server queue cant be removed");
        exit(EXIT_FAILURE);
    }
}



int main()
{

    if(atexit(shutdown_server) != 0) 
    {
        perror("Can't set exit function");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        perror("SIGINT handling couldn't be implemented");
        exit(EXIT_FAILURE);
    }
    
    key_t server_key = ftok(getenv("HOME"), SEED);

    if (server_key == -1) 
    {
        perror("Can't create unique key");
        exit(EXIT_FAILURE);
    }

    
    server_queue = msgget(server_key, IPC_CREAT | IPC_EXCL | 0666);

    if (server_queue == -1) 
    {
        perror("Server queue couldn't be openned");
        exit(EXIT_FAILURE);
    }

    task curr_task;

    for (int i = 0; i < MAXIMUM_CLIENTS; i++)
    {
        client_queues[i] = -1;
    }

    while (TRUE)
    {




        msgrcv(server_queue, &curr_task, MESSAGE_SIZE, -INIT, 0);
        process_task(&curr_task);
    }
    
    return 0;
}