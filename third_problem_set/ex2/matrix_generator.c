
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>




void create_matrix(char * file_name, int rows, int cols)
{
    FILE * file = fopen(file_name, "w");

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            char str[7];
            sprintf(str,"%d ", (rand() % 201) - 100);
            fwrite(str, sizeof(char), strlen(str), file);   
        }
        fseek(file, -1 * sizeof(char), SEEK_CUR);
        fwrite("\n", sizeof(char), strlen("\n"), file);   
        
    }
    fseek(file, -1 * sizeof(char), SEEK_CUR);
    fwrite("\0", sizeof(char), strlen("\0"), file);  
    fflush(file);

    fclose(file);
}

int main(int argc, char** argv)
{

    if(argc != 4)
    {
        printf("Wrong number of arguments\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    int size_min = atoi(argv[2]);
    int size_max = atoi(argv[3]);


    int seed;
    time_t tt;
    seed = time(&tt);
    srand(seed);

    char * list_name = "matrix_list.txt";
    FILE * list_file = fopen(list_name, "w");

    for (int i = 0; i < n; i++)
    {
        int A_rows = (rand() % (size_max - size_min + 1) ) + size_min;
        int A_cols = (rand() % (size_max - size_min + 1) ) + size_min;
        int B_cols = (rand() % (size_max - size_min + 1) ) + size_min;

        int len = snprintf(NULL, 0, "mat%dA.txt",i) + 1;
        char A_name[len];
        snprintf(A_name, len, "mat%dA.txt",i);

        len = snprintf(NULL, 0, "mat%dB.txt",i) + 1;
        char B_name[len];
        snprintf(B_name, len, "mat%dB.txt",i);

        len = snprintf(NULL, 0, "mat%dC.txt",i) + 1;
        char C_name[len];
        snprintf(C_name, len, "mat%dC.txt",i);

        FILE * file = fopen(C_name, "w");
        fclose(file);

        create_matrix(A_name, A_rows, A_cols);
        create_matrix(B_name, A_cols, B_cols);


        len = snprintf(NULL, 0, "%s %s %s\n",A_name, B_name, C_name) + 1;
        char line[len];
        snprintf(line, len, "%s %s %s\n",A_name, B_name, C_name);

        fwrite(line, sizeof(char), strlen(line), list_file);  
        fflush(list_file);


        

    }
    fclose(list_file);

    return 0;
}