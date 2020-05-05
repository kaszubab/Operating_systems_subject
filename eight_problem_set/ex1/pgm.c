#define _GNU_SOURCE
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include "math.h"
#include "time.h"
#include "unistd.h"

#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y)) 




void help_function()
{
    printf("\nUsage :  ./program <thread_count> <mode> <input> <output> \n");
    printf("thread_count - number of threads\n");
    printf("mode - mode of computing sign|block|interleaved\n");
    printf("input - input file name\n");
    printf("output - output file name\n");

}

int ** shade_matrix;
int * histogram;

int width;
int height;
int thread_count;

pthread_t * threads;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct pair
{
    int first;
    int second;
} pair;

struct timespec *time_diff(struct timespec start, struct timespec end)
{
    struct timespec *diff = (struct timespec *) calloc(1, sizeof(struct timespec));
    diff->tv_sec = end.tv_sec - start.tv_sec;
    diff->tv_nsec = end.tv_nsec - start.tv_nsec;

    if (start.tv_nsec > end.tv_nsec)
    {
        diff->tv_sec--;
        diff->tv_nsec = -diff->tv_nsec;
    }

    return diff;
}


void * sign_method(void * arg)
{
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    
    pair * my_interval = (pair *) arg;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int shade = shade_matrix[i][j];
            

            if (shade <= my_interval->second && shade >= my_interval->first)
            {
                histogram[shade]++;
            }
        }
    }
    
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec *total_time = time_diff(start, end);
    pthread_exit((void *)total_time);
}

void * block_method(void * arg)
{
    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    pair * bounds = (pair *) arg;

    for (int i = bounds->first; i < bounds->second; i++)
    {
        for (int j = 0; j < height; j++)
        {
            int shade = shade_matrix[j][i];

            pthread_mutex_lock(&mtx);

            histogram[shade]++;

            pthread_mutex_unlock(&mtx);
        }
    }

    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec *total_time = time_diff(start, end);
    return (void *)total_time;
}

void * interleaved_method(void * arg)
{
    pair * jump = (pair *) arg;

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);

    for (int j = 0; j < height; j++)
    {
        for (int i = jump->first; i < width; i += jump->second)
        {
            int shade = shade_matrix[j][i];
            pthread_mutex_lock(&mtx);
            histogram[shade]++;
            pthread_mutex_unlock(&mtx);
        }
    }

    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec *total_time = time_diff(start, end);
    return (void *)total_time;
}


void fill_shade_matrix(FILE * filename)
{
    char * line = NULL;
    size_t line_len = 0;

    if (getline(&line, &line_len, filename) == 0)
    {
        printf("Error while reading a file");
        exit(EXIT_FAILURE);
    }

    if (strcmp(line, "P2\n") != 0)
    {
        for ( int i = 0; i < line_len; i++)
        {
            printf("char %d -> %c", i, line[i]);
        }
        printf("Wrong format of the file %s",line);
        exit(EXIT_FAILURE);
    }



    if (getline(&line, &line_len, filename) == 0)
    {
        printf("Error while reading a file");
        exit(EXIT_FAILURE);
    }

    width = atoi(strtok(line, " "));
    height = atoi(strtok(NULL, " "));

    shade_matrix = (int **) calloc(height, sizeof(int *));
    
    for (int i = 0; i < height; i++)
    {
        shade_matrix[i] = (int *) calloc(width, sizeof(int));
    }


    if (getline(&line, &line_len, filename) == 0)
    {
        printf("Error while reading a file");
        exit(EXIT_FAILURE);
    }

    line_len = 0;
    line = NULL;

    int row = 0;
    int col = 0;

    while (getline(&line, &line_len, filename) >= 0)
    {
        char * number = strtok(line, " ");
        
   
        while(number != NULL)
        {
            shade_matrix[row][col++] = atoi(number);
            row += col/width;
            col %= width;
            number = strtok(NULL, " \n");
            fflush(stdout);
        }
       
    }

}

void save_hist(char ** argv)
{
    FILE * output_file = fopen(argv[4], "w");
    if (output_file == NULL)
    {
        printf("File error");
        exit(EXIT_FAILURE);
    }
    
    for (int i =0; i < 256; i++)
    {
        fprintf(output_file, "%d -> %d\n", i, histogram[i] );
    }

    fclose(output_file);
}


int main(int argc, char ** argv)
{
    

    if (argc != 5 && argc != 2)
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
    
    if (argc == 5)
    {
        thread_count = atoi(argv[1]);

        threads = (pthread_t *) calloc(thread_count, sizeof(pthread_t));

        FILE * input_file = fopen(argv[3], "r");

        if (input_file == NULL)
        {
            printf("File error");
            exit(EXIT_FAILURE);
        }

        fill_shade_matrix(input_file);
        histogram = (int *) calloc(256, sizeof(int));


        char * option = argv[2];

        struct timespec start;
        clock_gettime(CLOCK_REALTIME, &start);


        if (strcmp(option, "sign") == 0)
        {
            // printf("Option %s \n", option);
            pair * data = (pair *) calloc(thread_count, sizeof(pair));

            for (int i = 0 ; i < thread_count; i++)
            {
                int from = i * 256/thread_count;
                int to = ((i+1) * 256/thread_count) - 1;

                data[i].first = from;
                data[i].second = to;


                // printf("Interval outside %d %d \n", intrvl.first, intrvl.second );
                
                if(pthread_create(&(threads[i]), NULL, sign_method, (void *) &data[i]) != 0)
                {
                    printf("Thread creation error");
                    exit(EXIT_FAILURE);
                }

            }
            
        }
        else if(strcmp(option, "block") == 0)
        {
            pair * data = (pair *) calloc(thread_count, sizeof(pair));

            for (int i = 0 ; i < thread_count; i++)
            {
                int left = i * ceil(width/thread_count);
                int right = MIN( (i+1) * ceil(width/thread_count), width);

                data[i].first = left;
                data[i].second = right;
                
                if(pthread_create(&(threads[i]), NULL, block_method, (void *) &data[i]) != 0)
                {
                    printf("Thread creation error");
                    exit(EXIT_FAILURE);
                }
            }

        }
        else if(strcmp(option, "interleaved") == 0)
        {
            pair * data = (pair *) calloc(thread_count, sizeof(pair));

            for (int i = 0 ; i < thread_count; i++)
            {
                int start = i;
                int step = thread_count;

                data[i].first = start;
                data[i].second = step;
                
                if(pthread_create(&(threads[i]), NULL, interleaved_method, (void *) &data[i]) != 0)
                {
                    printf("Thread creation error");
                    exit(EXIT_FAILURE);
                }
            }

        }
        else
        {
            printf("Illegal option %s", option);
            exit(EXIT_FAILURE);
        }



        void *result;

        for (int i = 0; i < thread_count; i++)
        {
            pthread_join(threads[i], &result);
            struct timespec *tm = (struct timespec *) result;
            printf("Thread %ld: %lds %ldms\n", threads[i], tm->tv_sec, tm->tv_nsec / 1000);

        }

        
        struct timespec end;
        clock_gettime(CLOCK_REALTIME, &end);

        struct timespec *total_time = time_diff(start, end);
        printf("Total time: %lds %ldms\n", total_time->tv_sec, total_time->tv_nsec / 1000);
        


        save_hist(argv);
        fclose(input_file);
        
 
        
        
    }

    return 0;

}