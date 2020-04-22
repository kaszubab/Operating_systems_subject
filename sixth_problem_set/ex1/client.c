#include "stdio.h"
#include "stdlib.h"
#include "signal.h"
#include "unistd.h"
#include "string.h"

#include "queues.h"

int server_queue = -1;
int client_queue = -1;
int client_id = -1;
int caller_id = -1;
int active = FALSE;
int pending = TRUE;



void init(key_t client_queue_id)
{
    task init_task;
    init_task.type = INIT;
    init_task.client_pid = getpid();
    init_task.queue_id = client_queue_id;

    if (msgsnd(server_queue, &init_task, MESSAGE_SIZE,0) != 0)
    {
        printf("Init failure \n");
        exit(EXIT_FAILURE);
    }
    return;
}

void process_init(int cl_id)
{
    client_id = cl_id;
}

void stop()
{
    task stop_task;
    stop_task.type = STOP;
    stop_task.client_id = client_id;

    if (msgsnd(server_queue, &stop_task, MESSAGE_SIZE,0) < 0)
    {
            perror("Cant stop");
            exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

void estabilish_connection(key_t caller_k)
{
    caller_id = msgget(caller_k, IPC_CREAT);

    if (caller_id == -1)
    {
        printf("Cant connect");
        exit(EXIT_FAILURE);
    }

    pending = FALSE;
}

void run()
{
    char *cmd = (char *)calloc(MESSAGE_LEN, sizeof(char));

    fgets(cmd, MESSAGE_LEN, stdin);
    cmd = strtok(cmd, "\n");
    if (strcmp(cmd, "STOP") == 0)
    {
        stop();
    }
    else if (strcmp(cmd, "DISCONNECT") == 0)
    {
        task new_task;
        new_task.client_id = client_id;
        new_task.type  = DISCONNECT;

        if (msgsnd(server_queue, &new_task, MESSAGE_SIZE, 0) != 0)
        {
            printf("can't send message\n");
            exit(1);
        }
        pending = FALSE;
    }
    else if (strcmp(cmd, "LIST") == 0)
    {
        task new_task;
        new_task.client_id = client_id;
        new_task.type  = LIST;

        if (msgsnd(server_queue, &new_task, MESSAGE_SIZE, 0) != 0)
        {
            printf("can't send message\n");
            exit(1);
        }
        pending = FALSE;
    }
    else
    {

        char *buff = strtok(cmd, " ");
        if (strcmp(buff, "CONNECT") == 0)
        {
            buff = strtok(NULL, " ");
            task new_task;
            new_task.client_id = client_id;
            new_task.type  = CONNECT;
            strcpy(new_task.task_data, buff);
    

            if (msgsnd(server_queue, &new_task, MESSAGE_SIZE, 0) != 0)
            {
                printf("can't send message\n");
                exit(1);
            }
            pending = TRUE;
        }
        else
        {
            printf("\nNo such command!\n");
            pending = FALSE;
        }
    }

    free(cmd);
}

void process_task(task * curr_task)
{

    switch (curr_task->type)
    {
    case INIT:
        active = TRUE;
        pending = FALSE;
        process_init(curr_task->client_id);
        break;

    case STOP:
        printf("Stopping client \n");
        stop();
        break;

    case CONNECT:
        printf("Connected with client\n");
        estabilish_connection(curr_task->queue_id);
        break;
    
    default:
        break;
    }
}



void sigint_handler(int sig)
{
    printf("Exiting client and closing all open queues. Goodbye! \n");
    fflush(stdout);
    stop();

}





void shutdown_client()
{

    if (msgctl(client_queue, IPC_RMID, NULL))
    {
        perror("Client queue cant be removed");
        exit(EXIT_FAILURE);
    }

    
}

void sigusr_handler()
{
    task new_task;
    
    if (msgrcv(client_queue, &new_task, MESSAGE_SIZE, -INIT, 0) == -1)
    {
        perror("Can't receive message");
        shutdown_client();
        exit(1);
    }
    
    process_task(&new_task);

}




int main()
{

    if(atexit(shutdown_client) != 0) 
    {
        perror("Can't set exit function");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGINT, sigint_handler) == SIG_ERR)
    {
        perror("SIGINT handling couldn't be implemented");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGUSR1, sigusr_handler) == SIG_ERR)
    {
        perror("SIGUSR handling couldn't be implemented");
        exit(EXIT_FAILURE);
    }
    
    key_t server_key = ftok(getenv("HOME"), SEED);

    if (server_key == -1) 
    {
        perror("Can't create unique server key");
        exit(EXIT_FAILURE);
    }

    
    server_queue = msgget(server_key, IPC_CREAT  | 0666);


    if (server_queue == -1) 
    {
        perror("Server queue couldn't be openned");
        exit(EXIT_FAILURE);
    }

    key_t client_key = ftok(getenv("HOME"), getpid());

    if (client_key == -1) 
    {
        perror("Can't create unique client key");
        exit(EXIT_FAILURE);
    }

    
    client_queue = msgget(client_key, IPC_CREAT  | IPC_EXCL | 0666);

    if (client_queue == -1) 
    {
        perror("Client queue couldn't be openned");
        exit(EXIT_FAILURE);
    }

    
    init(client_key);

    task curr_task; 
    
    while (TRUE)
    {
        if (pending)
        {
            if (msgrcv(client_queue, &curr_task, MESSAGE_SIZE, -INIT, 0) == -1)
            {
                perror("Cant receive message");
                exit(EXIT_FAILURE);
            }
            process_task(&curr_task);
        }
        else
        {
            run();
        }
        
    }
    
    return 0;
}