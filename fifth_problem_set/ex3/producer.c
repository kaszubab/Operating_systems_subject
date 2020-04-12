#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <sys/types.h>
#include <unistd.h>

void help_function()
{
    printf("\nUsage :  ./program <stream_name> <file_name> <Buf_size>\n");
    printf("stream_name - name of FIFO queue \n");
    printf("file_name - name of the input file \n");
    printf("Buf_size - read buffer size \n");
}




int main(int argc, char ** argv)
{
    if (argc == 2)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            help_function();
        }
        else 
        {
            printf("Wrong argument. Use --help to get help on function");
        }

    }
    else if(argc == 4)
    {


        FILE * FIFO = fopen(argv[1], "w");

        if (FIFO == NULL)
        {
            printf("Stream %s couldnt be openned", argv[1]);
        }



        FILE *fp = fopen(argv[2], "r");

        if (fp == NULL)
        {
            printf("FIle %s couldnt be openned", argv[2]);
        }

        int buf_size = atoi(argv[3]);
        char buff[buf_size]; 

        

        while(fgets(buff, buf_size, fp))
        {
            
            fprintf(FIFO, "#%d#(%s)", getpid(), buff);
            sleep(1);

        }

        fclose(fp);
        fclose(FIFO);

    }
    else
    {
        printf("Wrong number of arguments. Use --help to get help on function");
    }
    return 0;
}