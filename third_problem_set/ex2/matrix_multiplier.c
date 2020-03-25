#define _GNU_SOURCE

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "limits.h"
#include <sys/file.h>
#include "unistd.h"


int multiplications = 0;

void get_row(FILE * matrix, int * line, int rows, int cols)
{
    char * single_line = NULL;
    size_t line_size = 0;

    getline(&single_line, &line_size, matrix);
    
    line[0] = atoi(strtok(single_line, " "));
    
    for (int i = 1; i < cols; i++)
    {
        line[i] = atoi(strtok(NULL, " "));
    }
}

void get_column(FILE * matrix, int * line, int rows, int cols, int index)
{
    fseek(matrix, 0, 0);
    char * single_line = NULL;
    size_t line_size = 0;

    char * one_digit;
    int row_num = 0;

    while(getline(&single_line, &line_size, matrix) >= 0)
    {
        one_digit = strtok(single_line, " ");
        if (index == 0)
        {
            line[row_num] = atoi(one_digit);
        }
        else
        {
            for (int i = 1; i <= index; i++)
            {
                one_digit = strtok(NULL, " ");
            }
            line[row_num] = atoi(one_digit);
        }
        row_num++;
    }
}

int dot_product(int * row, int * column, int len)
{
    int result = 0;
    for (int i = 0; i < len; i++)
    {
        result += row[i] * column[i];
    }
    return result;
}

void multiply_matrix_by_column(FILE * matrix_A, FILE * matrix_B, int * result_column, int index, int A_rows, int A_cols, int B_rows, int B_cols)
{
    int * column = (int *) calloc(B_rows, sizeof(int));
    int * row = (int*) calloc(A_cols, sizeof(int));

    get_column(matrix_B, column, B_rows, B_cols, index);
    fseek(matrix_A, 0, 0);

    for (int i = 0; i < A_rows; i++)
    {
        get_row(matrix_A, row, A_rows, A_cols);
        result_column[i] = dot_product(row, column, A_cols);
    }
}

void check_if_killed(int descriptor, FILE * file, int index)
{
    flock(descriptor, LOCK_EX);
    fseek(file, 2 * sizeof(char) * index, 0);
    int c = fgetc(file) - '0';
    if (c == 0)
    {
        flock(descriptor, LOCK_UN);
        exit(multiplications);
    }

    flock(descriptor, LOCK_UN);
}

int main(int argc, char ** argv) // matrix_multiplier matrix_A matrix_B matrix_C temp_file  index step mode A_rows A_cols B_rows B_cols
{

    if(argc != 12)
    {
        printf("Wrong number of arguments");
        exit(0);
    }
  
    FILE * matrix_A = fopen(argv[1], "r");

    if (matrix_A == NULL)
    {
        printf("File %s couldnt be openned", argv[1]);
        exit(0);
    }

    FILE * matrix_B = fopen(argv[2], "r");

    if (matrix_B == NULL)
    {
        printf("File %s couldnt be openned", argv[2]);
        exit(0);
    }

    int index = atoi(argv[5]);
    int step = atoi(argv[6]);
    
    int A_rows = atoi(argv[8]);
    int A_columns = atoi(argv[9]);

    int B_rows = atoi(argv[10]);
    int B_columns = atoi(argv[11]);

    char * mode = argv[7];


    FILE * temp_file = fopen(argv[4], "r+");

    while (temp_file == NULL)
    {
        printf("error");
        temp_file = fopen(argv[4], "r+");
        usleep(1);
    }

    


    int temp_descriptor = fileno(temp_file);

    flock(temp_descriptor, LOCK_UN);
    


    check_if_killed(temp_descriptor, temp_file, index);
    int * result_column = (int *) calloc(A_rows, sizeof(int));


    char **buf = (char **) calloc(A_rows, sizeof(char *));
    size_t *lens = (size_t *) calloc(A_rows, sizeof(size_t));

    for (int i = 0; i < A_rows; i++) {
        buf[i] = NULL;
        lens[i] = 0;
    }


    for (int i = index; i  < B_columns; i += step)
    {
        check_if_killed(temp_descriptor, temp_file, index);
        multiply_matrix_by_column(matrix_A, matrix_B, result_column, i,A_rows,A_columns,B_rows,B_columns);

        int last_column = -2;

        while(last_column != i-1)
        {
            check_if_killed(temp_descriptor, temp_file, index);
            
            flock(temp_descriptor, LOCK_EX);
           
            fseek(temp_file, 2 * step * sizeof(char), 0);

            char * line = NULL;
            size_t line_size = 0;

            while(getline(&line, &line_size, temp_file) >= 0)
            {
                last_column++;
            }

            
            flock(temp_descriptor, LOCK_UN);
            fflush(stdout);
            
           
        }

        if (strcmp(mode, "shared") == 0)
        {
        
        
            FILE * result_file = fopen(argv[3], "r+");

            if (result_file == NULL)
            {
                printf("Couldnt open %s  result file", argv[3]);
                exit(multiplications);
            }
            

            int result_descriptor = fileno(result_file);

            flock(result_descriptor, LOCK_UN);

            flock(result_descriptor, LOCK_EX);
            multiplications++;

        
            fflush(stdout);

            fseek(result_file, 0, 0);

            for (int j = 0; j < A_rows; j++) 
            {
                    char * line = NULL;
                    size_t line_size = 0;

                    lens[j] = getline(&line, &line_size, result_file);
                    buf[j] = line;

                    if(lens[j] == EOF)
                    {
                        buf[j] = NULL;
                        break;
                    }

                    if(lens[j] == 0)
                    {
                        buf[j][lens[j]] = '\0';
                    }

                    if (lens[j] > 0) 
                    {
                        buf[j][lens[j]-1] = ' ';
                        buf[j][lens[j]] = '\0';
                    }


                }
                
                fseek(result_file, 0, 0);

                for (int j = 0; j < A_rows; j++) 
                {
                    if (buf[j] != NULL) 
                    {
                        fprintf(result_file, "%s%d\n", buf[j], result_column[j]);
                    } 
                    else 
                    {
                        fprintf(result_file, "%d\n", result_column[j]);
                    }
                    fflush(result_file);
                }
                fflush(result_file);
                fclose(result_file);

                for (int j = 0; j < A_rows; j++) {
                    free(buf[j]);
                    buf[j] = NULL;
                }
        

            flock(result_descriptor, LOCK_UN);

            flock(temp_descriptor, LOCK_EX);
            fseek(temp_file, 0, SEEK_END);
            char str[7];
            sprintf(str,"%d\n", i);
            fwrite(str, sizeof(char), strlen(str), temp_file);
            fflush(result_file);
            flock(temp_descriptor, LOCK_UN);
        }
        else
        {
            int len = snprintf(NULL, 0, "/tmp/%d", i) + 1;
            char *path = calloc(len, sizeof(char));

            snprintf(path, len, "/tmp/%d", i);

            FILE *f = fopen(path, "w");

            if (f == NULL) 
            {
                printf("Can't open file to write partial result\n");
                exit(multiplications);
            }

            for (int j = 0; j < A_rows; j++) 
            {
                fprintf(f, "%d\n", result_column[j]);
            }

            fflush(f);
            fclose(f);
        }

         
    }
    


    fclose(temp_file);
    exit(multiplications);
        

    return 0;
}
