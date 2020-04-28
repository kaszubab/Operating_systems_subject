#define _POSIX_C_SOURCE 199309L

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

#include <time.h>


int sem_id;
int mem_id;
company_orders * orders;


void sigint_handler(int sig)
{   

    exit(EXIT_SUCCESS);
}

void say(int size, int prepare, int send)
{
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);

    printf("(%d %ld %ldms) Dodałem liczbę : %d. Liczba zamównień do przygotowania: %d. Liczba zamównień do wysłania: %d. \n", getpid(), tm.tv_sec, tm.tv_nsec%1000, size, prepare, send);
}

void retire()
{

    if (shmdt(orders) == -1)
    {
        printf("memory detach error");
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char ** argv)
{
    if (argc != 1)
     {
        printf("Program takes no arguments");
        exit(EXIT_FAILURE);
    }


    if(atexit(retire) != 0) 
    {
        perror("Can't set exit function");
        exit(EXIT_FAILURE);
    }

    if(signal(SIGUSR1, sigint_handler) == SIG_ERR)
    {
        perror("SIGINT handling couldn't be implemented");
        exit(EXIT_FAILURE);
    }



    key_t key = ftok(getenv("HOME"), SEED);

    if (key == -1) 
    {
        perror("Can't create unique key");
        exit(EXIT_FAILURE);
    }

    mem_id = shmget(key, sizeof(company_orders), IPC_CREAT );

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


    sem_id = semget(key, 4,  IPC_CREAT);

    if (sem_id == -1) 
    {
        printf("Semaphores error");
        exit(EXIT_FAILURE);
    }

    struct sembuf options;
    options.sem_flg = 0;

    srand(getpid());
    
    while(TRUE)
    {
        options.sem_num = 1;
        options.sem_op = -1;
        if (semop(sem_id, &options, 1) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }


        options.sem_num = 0;
        options.sem_op = -1;
        if (semop(sem_id, &options, 1) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }

        int order_size = rand()%100;
        orders->orders[orders->new_order_index] = order_size;
        orders->new_order_index = (orders->new_order_index + 1) % MAXIMUM_ORDERS;
        
        int prepare = semctl(sem_id, 2, GETVAL);
        int send = semctl(sem_id, 3, GETVAL);


        say(order_size, prepare, send);

        options.sem_num = 2;
        options.sem_op = 1;
        if (semop(sem_id, &options, 1) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }

        options.sem_num = 0;
        options.sem_op = 1;
        if (semop(sem_id, &options, 1) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }

        sleep((rand() % 3) +1);
        
    }

    return 0;

}