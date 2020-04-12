#include "stdio.h"
#include "string.h"
#include "stdlib.h"

void help_function()
{
    printf("\nUsage :  ./program <stream_name> <file_name> <Buf_size>\n");
    printf("stream_name - name of FIFO queue \n");
    printf("file_name - name of the result file \n");
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
        FILE * FIFO = fopen(argv[1], "r");

        if (FIFO == NULL)
        {
            printf("Stream %s couldnt be openned", argv[1]);
        }


        FILE *fp = fopen(argv[2], "w");

        if (fp == NULL)
        {
            printf("FIle %s couldnt be openned", argv[2]);
        }

        int buf_size = atoi(argv[3]);
        char buff[buf_size]; 

        

        while(fgets(buff, buf_size, FIFO))
        {
            
            fputs(buff, fp);

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