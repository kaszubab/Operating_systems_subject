#define _GNU_SOURCE

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"


void help_function()
{
    printf("\nUsage :  ./program <file_name> \n");
    printf("filename - name of the file containing lines to sort \n");
}




int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments. Use --help to get help on function");
    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        help_function();
    }
    else
    {
        FILE * sort = popen("sort", "w");

        FILE * fp = fopen(argv[1], "r");

        if (sort == NULL)
        {
            printf("Popen failure");
            exit(EXIT_FAILURE);
        }

        if (fp == NULL)
        {
            printf("File %s couldnt be openned", argv[1]);
            exit(EXIT_FAILURE);
        }

        char buff[BUFSIZ];

        while(fgets(buff, BUFSIZ, fp))
        {
            fputs(buff, sort);
        }

        pclose(sort);

        fclose(fp);


    }

    return 0;
}
    