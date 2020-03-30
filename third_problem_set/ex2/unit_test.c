#define _GNU_SOURCE

#include "limits.h"
#include <sys/file.h>
#include "unistd.h"
#include "sys/wait.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>





int main() 
{

    int n = 5;

    int seed;    
    time_t tt;
    seed = time(&tt);
    srand(seed);

    char * list_name = "matrix_list.txt";
    FILE * list_file = fopen(list_name, "w");



    int ** matrix_A;
    int ** matrix_B;

    matrix_A = (int **) calloc(5, sizeof(int*));
    matrix_B = (int **) calloc(5, sizeof(int*));
    int** res = calloc(5, sizeof(int*));

    for (int i = 0; i < 5; i++)
    {
        matrix_A[i] = (int *) calloc(5, sizeof(int));
        matrix_B[i] = (int *) calloc(5, sizeof(int));
    }


   

    for (int count = 0; count < n; count++) 
    {
        int len = snprintf(NULL, 0, "mat%dA.txt",count) + 1;
        char A_name[len];
        snprintf(A_name, len, "mat%dA.txt",count);

        len = snprintf(NULL, 0, "mat%dB.txt",count) + 1;
        char B_name[len];
        snprintf(B_name, len, "mat%dB.txt",count);

        len = snprintf(NULL, 0, "mat0C.txt") + 1;
        char C_name[len];
        snprintf(C_name, len, "mat0C.txt");

        FILE * file = fopen(C_name, "w");
        fclose(file);

        FILE * file1 = fopen(A_name, "w");
        FILE * file2 = fopen(B_name, "w");
        
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                matrix_A[i][j] = (rand() %201) - 100;
                matrix_B[i][j] = (rand() %201) - 100;

                char str[7];
                sprintf(str,"%d ", matrix_A[i][j] );
                fwrite(str, sizeof(char), strlen(str), file1);
                sprintf(str,"%d ", matrix_B[i][j] );
                fwrite(str, sizeof(char), strlen(str), file2);

            }
            fseek(file1, -1 * sizeof(char), SEEK_CUR);
            fwrite("\n", sizeof(char), strlen("\n"), file1);   
            fseek(file2, -1 * sizeof(char), SEEK_CUR);
            fwrite("\n", sizeof(char), strlen("\n"), file2);  
            
        }
        fseek(file1, -1 * sizeof(char), SEEK_CUR);
        fwrite("\0", sizeof(char), strlen("\0"), file1);  
        fflush(file1);

        fseek(file2, -1 * sizeof(char), SEEK_CUR);
        fwrite("\0", sizeof(char), strlen("\0"), file2);  
        fflush(file2);

        fclose(file1);
        fclose(file2);


        len = snprintf(NULL, 0, "%s %s %s\n",A_name, B_name, C_name) + 1;
        char line[len];
        snprintf(line, len, "%s %s %s\n",A_name, B_name, C_name);

        fwrite(line, sizeof(char), strlen(line), list_file);  
        fflush(list_file);

      

        int pid;

        if ((pid = fork()) == 0) 
        {
            execl("./program", "./program", list_name, "5", "10", "shared", NULL);
            exit(0);
        }

 
        

        for (int ii = 0;ii<5;ii++)
        {
            res[ii] = calloc(5, sizeof(int));
            for(int jj = 0;jj<5;jj++)
            {
                res[ii][jj] = 0;
                for(int kk = 0;kk<5;kk++)
                {
                    res[ii][jj] +=matrix_A[ii][kk] * matrix_B[kk][jj];
                }
            }
        }

        int status;
        wait(&status);

        file = fopen("mat0C.txt", "r");

        for (int ii = 0;ii<5;ii++) 
        {
            for (int jj = 0; jj < 5; jj++) 
            {
                int t;
                fscanf(file, "%d", &t);
                if(t != res[ii][jj])
                {
                    printf("Wrong result\n");
                    exit(1);
                }
            }
        }
        
        fclose(file);

        printf("test passed\n");
        system("rm -r mat0C.txt");

        return 0;
    }

    
}