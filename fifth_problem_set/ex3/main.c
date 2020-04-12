#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void help_function()
{
    printf("\nUsage :  ./program <output_file> <test> <test> <test> <test> <test>\n");
    printf("output_file - name of the output file \n");
    printf("test - test input file \n");
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
    else if(argc == 7)
    {


        if (mkfifo("Fifo", 0666) == 01)
        {
            printf("Fifo error");
            exit(EXIT_FAILURE);
        }

        pid_t pid ;

        if ((pid = fork()) < 0)
        {
            printf("Fork error");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            execl("./consumer", "./consumer", "Fifo", argv[1], "5", NULL);
        }

        
        
        for (int i = 0; i < 5; i++)
        {
            if((pid = fork()) == 0)
            {
                char buffer[2];
                sprintf(buffer, "%d", i+2);
                execl("./producer", "./producer", "Fifo", argv[2+i], buffer, NULL);

            }
        }


    }
    else
    {
        printf("Wrong number of arguments. Use --help to get help on function");
    }
    return 0;
}