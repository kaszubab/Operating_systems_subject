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
#include "time.h"


sem_t * sem_ids[4];
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

    if (munmap(orders, sizeof(company_orders)) == -1)
    {
        printf("memory detach error");
        exit(EXIT_FAILURE);
    }

    for (int i =0; i < 4; i++)
    {
        if (sem_close(sem_ids[i]) == -1)
        {
            printf("memory detach error");
            exit(EXIT_FAILURE);
        }
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



    mem_id = shm_open(SHARED_MEMORY, O_RDWR, 0);

    if (mem_id == -1) 
    {
        printf("Shared memory error");
        exit(EXIT_FAILURE);
    }

    orders = mmap(NULL, sizeof(company_orders), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0); 

    if (orders == MAP_FAILED) 
    {
        printf("mmap error");
        exit(EXIT_FAILURE);
    }
    


    sem_ids[0] = sem_open(OPERATION_LOCK,O_RDWR);

    if (sem_ids[0] == SEM_FAILED) 
    {
        printf("Semaphores1 error");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    sem_ids[1] = sem_open(SIZE_CONTROL,O_RDWR);

    if (sem_ids[1] == SEM_FAILED) 
    {
        printf("Semaphores2 error");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    sem_ids[2] = sem_open(WAITING_TO_BE_PROCESSED,O_RDWR);

    if (sem_ids[2] == SEM_FAILED) 
    {
        printf("Semaphores3 error");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    sem_ids[3] = sem_open(WAITING_FOR_SENDING,O_RDWR);

    if (sem_ids[3] == SEM_FAILED) 
    {
        printf("Semaphores3 error");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }


    srand(getpid());
    
    while(TRUE)
    {

        if (sem_wait(sem_ids[2]) == -1) 
        {
            printf("sem_wait error");
            exit(EXIT_FAILURE);
        }

        if (sem_wait(sem_ids[0]) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }

        
        int order_size = orders->orders[orders->prepare_order_index];
        orders->prepare_order_index = (orders->prepare_order_index + 1) % MAXIMUM_ORDERS;
        
        int prepare = 0;

        if (sem_getvalue(sem_ids[2], &prepare) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }

        int send = 0;

        if (sem_getvalue(sem_ids[3], &send) == -1) 
        {
            printf("Semop error");
            exit(EXIT_FAILURE);
        }


        say(order_size*2, prepare, send+1);

        if (sem_post(sem_ids[3])== -1) 
        {
            printf("Sem_wait error");
            exit(EXIT_FAILURE);
        }
        
        if (sem_post(sem_ids[0])== -1) 
        {
            printf("Sem_wait error");
            exit(EXIT_FAILURE);
        }
        
    }

    return 0;

}