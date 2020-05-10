#define _GNU_SOURCE

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include "math.h"
#include "time.h"
#include "unistd.h"

#include "header.h"


#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y)) 




void help_function()
{
    printf("\nUsage :  ./program <number_of_chairs> <number_of_clients> \n");
}


int thread_count;

pthread_t * clients;
pthread_t barber;
int barber_working = FALSE;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

client_queue * queue;

typedef struct pair
{
    int first;
    int second;
} pair;

void * be_client(void * arg)
{
    while(TRUE)
    {
        if (!queue_is_full(queue))
        {
            pthread_mutex_lock(&mtx);
            add_client(queue, pthread_self());
            int free_places = queue->max_len - queue->curr_len;
            if (barber_working == FALSE)
            {
                printf("Budze golibrode; %ld \n", pthread_self());
                pthread_cond_signal(&cond);
            }
            else
            {
                printf("Poczekalnia, wolne miejsca: %d; %ld \n", free_places,  pthread_self() );
            }
            pthread_mutex_unlock(&mtx);
            break;
        }
        else 
        {
            printf("Zajete; %ld \n", pthread_self());
            int r = rand()%5 + 1;
            sleep(r);
        }
    }

    return NULL;
}

void cleanup_handler(void * arg)
{
    pthread_mutex_unlock(&mtx);
}

void * be_barber(void * arg)
{
    node * to_shave;

    pthread_cleanup_push(cleanup_handler, arg);

    while(TRUE)
    {
        pthread_mutex_lock(&mtx);
        while (queue_is_empty(queue))
        {
            printf("Golibroda: ide spac \n");
            barber_working = FALSE;
            pthread_cond_wait(&cond, &mtx);
        }
        to_shave = shave_client(queue);
        barber_working = TRUE;
        pthread_t client_id = to_shave->client_id;
        free(to_shave);
        // pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        printf("Golibroda: czeka %d klientow, gole klienta %ld \n", queue->curr_len, client_id);
        // pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_mutex_unlock(&mtx);
    
        int r = rand()%5 + 4;
        sleep(r);
 
        printf("Golenie zakończone\n");
    }
    
    pthread_cleanup_pop(0);

    return NULL;
}



int main(int argc, char ** argv)
{
    

    if (argc != 3 && argc != 2)
     {
        printf("Invalid number of arguments use --help to get help");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") != 0) 
        {
            printf("Illegal argument %s", argv[1]);
            exit(EXIT_FAILURE);
        }

        help_function();
        exit(EXIT_SUCCESS);
        
    }
    
    if (argc == 3)
    {
        thread_count = atoi(argv[2]);
        int chairs_count = atoi(argv[1]);

        clients = (pthread_t *) calloc(thread_count, sizeof(pthread_t));

        queue = queue_initialize(chairs_count);

        srand(time(NULL));  

        if(pthread_create(&barber, NULL, be_barber, NULL) != 0)
        {
            printf("Barber creation error");
            exit(EXIT_FAILURE);
        }

        int r = rand()%5 + 1;



        for (int i = 0 ; i < thread_count; i++)
        {
            sleep(r);
            
            if(pthread_create(&(clients[i]), NULL, be_client, NULL) != 0)
            {
                printf("Client creation error");
                exit(EXIT_FAILURE);
            }
            r = rand()%3 + 1;

        }
            
        for (int i = 0; i < thread_count; i++)
        {
            pthread_join(clients[i],NULL);
        }

        while (barber_working == TRUE)
        {
            sleep(1);
        }
        
        pthread_cancel(barber);
        pthread_join(barber, NULL);
        
    }

    return 0;

}