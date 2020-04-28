#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/sem.h"
#include "sys/ipc.h"
#include "sys/types.h"

#include "shared.h"

#include "sys/shm.h"
#include "unistd.h"
#include "sys/wait.h"
#include "signal.h"


int receivers;
int packers;
int senders;

int sem_id;
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

    for(int i = 0; i < MAXIMUM_ORDERS; i++)
    {
        printf("Order %d \n", orders->orders[i]);
    }

    printf("New order index %d \n",orders->new_order_index);

    if (shmdt(orders) == -1)
    {
        printf("memory detach error");
        exit(EXIT_FAILURE);
    }

    if (shmctl(mem_id, IPC_RMID, NULL) == -1)
    {
        printf("memory removal error");
        exit(EXIT_FAILURE);
    }

    if (semctl(sem_id, 0, IPC_RMID) == -1)
    {
        printf("semaphores removal error");
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



        key_t key = ftok(getenv("HOME"), SEED);

        if (key == -1) 
        {
            perror("Can't create unique key");
            exit(EXIT_FAILURE);
        }

        mem_id = shmget(key, sizeof(company_orders), ACCESS_FLAGS | IPC_CREAT | IPC_EXCL);

        if (mem_id == -1) 
        {
            printf("Shared memory error");
            exit(EXIT_FAILURE);
        }


        orders = (company_orders *) shmat(mem_id, NULL, 0);

        if (orders == (company_orders *)-1) 
        {
            printf("SHMAT error");
            exit(EXIT_FAILURE);
        }

        orders->new_order_index = orders->prepare_order_index = orders->send_order_index = 0;
        // orders->orders_to_prepare = orders->orders_to_send = 0;
        


        sem_id = semget(key, 4, ACCESS_FLAGS | IPC_CREAT | IPC_EXCL);

        if (sem_id == -1) 
        {
            printf("Semaphores error");
            exit(EXIT_FAILURE);
        }

        semun sem1;

        sem1.val = 1;
        semctl(sem_id, 0, SETVAL, sem1);

        sem1.val = MAXIMUM_ORDERS;
        semctl(sem_id, 1, SETVAL, sem1);

        sem1.val = 0;
        semctl(sem_id, 2, SETVAL, sem1);
        semctl(sem_id, 3, SETVAL, sem1);

        
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