#define _POSIX_C_SOURCE 200112L
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "sys/wait.h"
#include "signal.h"

#include "shared.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "fcntl.h"

#include "semaphore.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/types.h"



int receivers;
int packers;
int senders;

sem_t * sem_ids[4];
int mem_id;
company_orders * orders;

pid_t * receivers_id;
pid_t * packers_id;
pid_t * senders_id;



void help_function()
{
    printf("\nUsage :  ./program <receivers> <packers> <senders> \n");
    printf("receivers - number of receivers\n");
    printf("packers - number of packers\n");
    printf("senders - number of senders\n");
}

void sigint_handler(int sig)
{
    printf("Company is going broke \n");
    fflush(stdout);
    

    for (int i = 0; i < receivers; i++)
    {
        if (kill(receivers_id[i], SIGUSR1) == -1)
        {
            printf("Kill error");
            exit(EXIT_FAILURE);
        }
        
    }


    for (int i = 0; i < packers; i++)
    {
        if (kill(packers_id[i], SIGUSR1) == -1)
        {
            printf("Kill error");
            exit(EXIT_FAILURE);
        }
        
    }

    for (int i = 0; i < senders; i++)
    {
        if (kill(senders_id[i], SIGUSR1) == -1)
        {
            printf("Kill error");
            exit(EXIT_FAILURE);
        }
        
    }



    exit(EXIT_SUCCESS);
}

void close_company()
{
    free(senders_id);
    free(receivers_id);
    free(packers_id);

    if (munmap(orders, sizeof(company_orders))  == -1)
    {
        printf("memory detach error");
        exit(EXIT_FAILURE);
    }

    if (shm_unlink(SHARED_MEMORY) == -1)
    {
        printf("memory removal error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 4; i++)
    {
        if (sem_close(sem_ids[i])  == -1)
        {
            printf(" \n %d \n",i);
            printf("Sem_close error");
            exit(EXIT_FAILURE);
        }
    }


    if (sem_unlink(OPERATION_LOCK) == -1)
    {
        printf("operation lock removal error");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(SIZE_CONTROL) == -1)
    {
        printf("size lock error");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(WAITING_TO_BE_PROCESSED) == -1)
    {
        printf("process lock error");
        exit(EXIT_FAILURE);
    }

    if (sem_unlink(WAITING_FOR_SENDING) == -1)
    {
        printf("send lock error");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char ** argv)
{
    

    if (argc != 4 && argc != 2)
     {
        printf("Invalid number of arguments use --help to get help");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0) 
        {
            help_function();
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Illegal argument %s", argv[1]);
            exit(EXIT_FAILURE);
        }
        
    }

    if (argc == 4) 
    {
        

        if(atexit(close_company) != 0) 
        {
            perror("Can't set exit function");
            exit(EXIT_FAILURE);
        }
        

        if(signal(SIGINT, sigint_handler) == SIG_ERR)
        {
            perror("SIGINT handling couldn't be implemented");
            exit(EXIT_FAILURE);
        }

        receivers = atoi(argv[1]);
        packers = atoi(argv[2]);
        senders = atoi(argv[3]);

        receivers_id = (pid_t *) calloc(receivers, sizeof(pid_t));
        packers_id = (pid_t *) calloc(packers, sizeof(pid_t));
        senders_id = (pid_t *) calloc(senders, sizeof(pid_t));


        mem_id = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT | O_EXCL, ACCESS_FLAGS);

        if (mem_id == -1) 
        {
            printf("Shared memory error");
            exit(EXIT_FAILURE);
        }

        if (ftruncate(mem_id, sizeof(company_orders)) == -1) 
        {
            printf("Truncate error");
            exit(EXIT_FAILURE);
        }

        orders = mmap(NULL, sizeof(company_orders), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0); 

        if (orders == MAP_FAILED) 
        {
            printf("mmap error");
            exit(EXIT_FAILURE);
        }

        orders->new_order_index = orders->prepare_order_index = orders->send_order_index = 0;

        sem_ids[0] = sem_open(OPERATION_LOCK, O_CREAT | O_EXCL, ACCESS_FLAGS, 1);

        if (sem_ids[0] == ((sem_t *)0)) 
        {
            printf("Semaphores1 error");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        sem_ids[1] = sem_open(SIZE_CONTROL,O_EXCL | O_CREAT, ACCESS_FLAGS ,MAXIMUM_ORDERS);

        if (sem_ids[1] == SEM_FAILED) 
        {
            printf("Semaphores2 error");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        sem_ids[2] = sem_open(WAITING_TO_BE_PROCESSED,O_EXCL | O_CREAT, ACCESS_FLAGS,0);

        if (sem_ids[2] == SEM_FAILED) 
        {
            printf("Semaphores3 error");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }

        sem_ids[3] = sem_open(WAITING_FOR_SENDING, O_EXCL | O_CREAT, ACCESS_FLAGS, 0);

        if (sem_ids[3] == SEM_FAILED) 
        {
            printf("Semaphores4 error");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }


       
        for (int i = 0; i < receivers; i++) 
        {
            receivers_id[i] = fork();
            if (receivers_id[i] == 0)
            {
                execl("./receiver", "./receiver", NULL);
            }
        }

        for (int i = 0; i < packers; i++) 
        {
            packers_id[i] = fork();
            if (packers_id[i] == 0)
            {
                execl("./packing", "./packing", NULL);
            }
        }

        for (int i = 0; i < senders; i++) 
        {
            senders_id[i] = fork();
            if (senders_id[i] == 0)
            {
                execl("./sender", "./sender", NULL);
            }
        }

        for (int i = 0; i < receivers; i++) 
        {
            waitpid(receivers_id[i], NULL, 0);
        }


        for (int i = 0; i < packers; i++) 
        {
            waitpid(packers_id[i], NULL, 0);
        }

        for (int i = 0; i < senders; i++) 
        {
            waitpid(senders_id[i], NULL, 0);
        }
 
    }

    return 0;

}