#define _GNU_SOURCE

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"
#include <sys/file.h>
#include "unistd.h"
#include "sys/wait.h"


typedef struct matrix_sizes
{
    int first_matrix_rows;
    int first_matrix_cols;
    int second_matrix_rows;
    int second_matrix_cols;
} matrix_sizes;



void print_help()
{
    printf("Usage of command: command  <file_name>   <processes_count>   <time_limit>    <mode>\n");
    printf("<file_name> - file with the names of A matrix, B matrix and result matrix in each line \n");
    printf("<process_count> - number of processes used to compute the matrix\n");
    printf("<time_limit> - upper bound on the process longevity\n");
    printf("<mode> - shared to save results from processes in one file or non_shared to save in different files\n");

}


int read_file_names(FILE * list, char ** matrices_A, char ** matrices_B, char ** matrices_C) // returns number of matrices or -1 if something went wrong
{
    int matrices_quantity = 0;

    char * line;
    line = (char *) calloc(200, sizeof(char));
    size_t line_size = 200;
    char * to_copy;


    while((fgets(line, line_size, list)) != NULL)
    {
        if(matrices_quantity > 0)
        {
            matrices_A = (char **) realloc(matrices_A, sizeof(char *) * (matrices_quantity+1));
            matrices_B = (char **) realloc(matrices_B, sizeof(char *) * (matrices_quantity+1));
            matrices_C = (char **) realloc(matrices_C, sizeof(char *) * (matrices_quantity+1));
        }

        int read_chars = strlen(line);

        matrices_A[matrices_quantity] = (char *) calloc(read_chars, sizeof(char));
        matrices_B[matrices_quantity] = (char *) calloc(read_chars, sizeof(char));
        matrices_C[matrices_quantity] = (char *) calloc(read_chars, sizeof(char));
        
        to_copy =  strtok(line, " ");
        strcpy(matrices_A[matrices_quantity], to_copy);

        to_copy =  strtok(NULL, " ");
        strcpy(matrices_B[matrices_quantity], to_copy);

        to_copy =  strtok(NULL, "\n");
        strcpy(matrices_C[matrices_quantity], to_copy);

        matrices_quantity++;

    }


    return matrices_quantity;
}


void get_sizes( char ** matrices_A, char ** matrices_B, matrix_sizes * sizes, int matrix_count)
{
    FILE * first_matrix;
    FILE * second_matrix;

    int a_size,b_size;

    char * line = NULL;
    size_t line_size = 0;


    for (int i = 0; i < matrix_count; i++)
    {
        if ((first_matrix = fopen(matrices_A[i],"r")) == NULL)
        {
            printf("Couldnt open file %s", matrices_A[i]);
            exit(1);
        }

        if ((second_matrix = fopen(matrices_B[i],"r")) == NULL)
        {
            printf("Couldnt open file %s", matrices_B[i]);
            exit(1);
        }

        a_size = 0;

        while(getline(&line, &line_size, first_matrix) >= 0)
        {
            a_size++;
        } 

        sizes[i].first_matrix_rows = a_size;

        fseek(first_matrix, 0, 0);

        getline(&line, &line_size, first_matrix);

        if ( strtok(line, " ") != NULL) 
        {
            a_size = 1;
            while(strtok(NULL, " ") != NULL) 
                a_size++;
        }

        sizes[i].first_matrix_cols = a_size;

        b_size = 0;

        while(getline(&line, &line_size, second_matrix) >= 0)
        {
            b_size++;
        } 

        sizes[i].second_matrix_rows = b_size;

        fseek(second_matrix, 0, 0);

        getline(&line, &line_size, second_matrix);

        if ( strtok(line, " ") != NULL) 
        {
            b_size = 1;
            while(strtok(NULL, " ") != NULL) 
                b_size++;
        }

        sizes[i].second_matrix_cols = b_size;


        if (sizes[i].first_matrix_cols != sizes[i].second_matrix_rows)
        {
            printf("Matrices %s and %s are of imcompatible sizes \n", matrices_A[i], matrices_B[i]);
            exit(1);
        }

    }   

}

int main(int argc, char ** argv ) 
{

    if (argc < 2)  
    {
        perror("Error parsing arguments, use --help to get the of available commands");
        exit(1);
    }


    if (argc == 2 && strcmp(argv[1], "--help") == 0) 
    {
        print_help();
        return 0;
    }

    if (argc != 5)  
    {
        perror("Error parsing arguments, use --help to get the of available commands");
        exit(1);
    }

    char * file_name = argv[1];
    FILE * input_file = fopen(file_name, "r");

    
    if (input_file == NULL) 
    {
        printf("File %s couldnt be openned", file_name);
        exit(1);
    }

    char ** matrices_A = (char **) calloc(1, sizeof(char *));
    char ** matrices_B = (char **) calloc(1, sizeof(char *));
    char ** matrices_C = (char **) calloc(1, sizeof(char *));

    int matrix_count = read_file_names(input_file, matrices_A, matrices_B, matrices_C);
    fclose(input_file);
    
    
    matrix_sizes * mat_sizes = (matrix_sizes *) calloc(matrix_count, sizeof(matrix_sizes));
    get_sizes(matrices_A, matrices_B, mat_sizes, matrix_count);
    
    
    int process_count = strtol(argv[2], NULL, 0);
    pid_t workers [process_count];



    double time_to_live = strtod(argv[3],NULL);

    char * mode = NULL;

    if( strcmp(argv[4], "shared") == 0)
    {
        mode = argv[4];
    }
    else if(strcmp(argv[4],"non_shared") == 0)
    {
        mode = argv[4];
    }

    if (mode == NULL)
    {
        perror("Error parsing mode argument, use --help to get the of available commands");
        exit(1);
    }

    char * temp_name = "temp.txt";

    FILE * tmp = fopen(temp_name,"w");

    for (int i = 0; i < process_count; i++)
    {
        fwrite("1\n", sizeof(char), strlen("1\n"), tmp);
    }

    fwrite("-1\n", sizeof(char), strlen("-1\n"), tmp);
    fflush(tmp);
    flock(fileno(tmp), LOCK_UN);

    int len = snprintf(NULL, 0, "%d", process_count)+1;
    char step[len];
    snprintf(step, len, "%d", process_count);


    for (int i = 0; i < process_count; i++)
    {
        if ((workers[i] = fork()) == 0)
        {
 
            int len = snprintf(NULL, 0, "%d", i)+1;
            char child_id[len];
            snprintf(child_id, len, "%d", i);

            len = snprintf(NULL, 0, "%d", mat_sizes[0].first_matrix_rows)+1;
            char A_rows[len];
            snprintf(A_rows, len, "%d", mat_sizes[0].first_matrix_rows);

            len = snprintf(NULL, 0, "%d", mat_sizes[0].first_matrix_cols)+1;
            char A_cols[len];
            snprintf(A_cols, len, "%d", mat_sizes[0].first_matrix_cols);

            len = snprintf(NULL, 0, "%d", mat_sizes[0].second_matrix_rows)+1;
            char B_rows[len];
            snprintf(B_rows, len, "%d", mat_sizes[0].second_matrix_rows);

            len = snprintf(NULL, 0, "%d", mat_sizes[0].second_matrix_cols)+1;
            char B_cols[len];
            snprintf(B_cols, len, "%d", mat_sizes[0].second_matrix_cols);
 
            execl("./matrix_multiplier", "./matrix_multiplier", matrices_A[0], matrices_B[0], matrices_C[0], temp_name, child_id, step, mode, A_rows, A_cols, B_rows, B_cols, NULL); 
        
        }
    }

    
    
    int sleep_time = 50000; //50 ms
    int timeout = time_to_live * 1000000;
    int time = 0;
    int num_files = 0;

    int multiplications = 0;

    for(int i = 0;i<process_count;i++)
    {
        while(time < timeout)
        {
            if(waitpid(workers[i], &multiplications, WNOHANG) != 0)
            {
                printf("Process with PID %d performed %d multiplications\n", workers[i], WEXITSTATUS(multiplications));
                break;
            } 
            else 
            {
                usleep(sleep_time);
                time += sleep_time;
            }
        }

        if(time >= timeout)
        {
            flock(fileno(tmp), LOCK_EX);
            fseek(tmp, 2 * i *sizeof(char), 0);
            fprintf(tmp, "0\n");
            fflush(tmp);
            flock(fileno(tmp), LOCK_UN);

            waitpid(workers[i], &multiplications, 0);
            printf("Process with PID %d performed %d multiplications before being killed\n", workers[i], WEXITSTATUS(multiplications));
        }

        num_files += WEXITSTATUS(multiplications);
    }

    

    if(strcmp(mode, "non_shared") == 0)
    {
        FILE * result_file = fopen(matrices_C[0], "w+");

        if(fork() == 0)
        {
            char** params = (char**) calloc(num_files+4, sizeof(char*));
            params[0] = "/usr/bin/paste"; 
            params[1] = "-d";
            params[2] = " ";

            if(result_file == NULL)
            {
                printf("Error opening output file\n");
                exit(1);
            }

            dup2(fileno(result_file), 1);
            int len;
            for(int i = 0;i<num_files; i++)
            {
                len = snprintf(NULL, 0, "tmp/%d", i)+1;
                params[i+3] = calloc(len, sizeof(char));
                snprintf(params[i+3], len, "tmp/%d", i);
            }

            params[num_files+3] = NULL;

            execv(params[0], params);
            exit(0);
        }
        wait(0);
    }

    return 0;
}