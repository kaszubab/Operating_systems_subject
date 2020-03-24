#define _GNU_SOURCE

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"


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


void get_sizes( char ** matrices_A, char ** matrices_B, int * sizes, int matrix_count)
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

        getline(&line, &line_size, first_matrix);

        if ( strtok(line, " ") != NULL) 
        {
            a_size = 1;
            while(strtok(NULL, " ") != NULL) 
                a_size++;
        }

        b_size = 0;

        while(getline(&line, &line_size, second_matrix) >= 0)
        {
            b_size++;
        } 

        if (a_size != b_size)
        {
            printf("Matrices %s and %s are of imcompatible sizes \n", matrices_A[i], matrices_B[i]);
            exit(1);
        }
        else
        {
            sizes[i] = a_size;
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
    
    
    int * matrix_sizes = (int *) calloc(matrix_count, sizeof(int));
    get_sizes(matrices_A, matrices_B, matrix_sizes, matrix_count);
    
    
    int process_count = strtol(argv[2], NULL, 0);
    pid_t workers [process_count];



    double time_to_live = strtod(argv[3],NULL);

    int mode = -1;

    if( strcmp(argv[4], "shared") == 0)
    {
        mode = 0;
    }
    else if(strcmp(argv[4],"non_shared") == 0)
    {
        mode = 1;
    }

    if (mode == -1)
    {
        perror("Error parsing mode argument, use --help to get the of available commands");
        exit(1);
    }


    for (int i = 0; i < process_count; i++)
    {
        // TODO start process
    }
  

    return 0;
}